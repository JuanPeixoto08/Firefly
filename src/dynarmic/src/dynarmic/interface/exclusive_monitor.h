// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <boost/container/static_vector.hpp>

#include <dynarmic/common/spin_lock.h>

namespace Dynarmic {

using VAddr = std::uint64_t;

#if defined(__cpp_lib_atomic_ref)
template<typename T>
using AtomicRef = std::atomic_ref<T>;
#else
/// std::atomic_ref for standard libraries without it (Android's NDK libc++).
template<typename T>
class AtomicRef {
public:
    explicit AtomicRef(T& value_) : value{value_} {}
    T load(std::memory_order order = std::memory_order_seq_cst) const {
        return __atomic_load_n(&value, static_cast<int>(order));
    }
    void store(T desired, std::memory_order order = std::memory_order_seq_cst) const {
        __atomic_store_n(&value, desired, static_cast<int>(order));
    }
    bool compare_exchange_strong(T& expected, T desired,
                                 std::memory_order order = std::memory_order_seq_cst) const {
        const int failure = order == std::memory_order_acq_rel   ? __ATOMIC_ACQUIRE
                            : order == std::memory_order_release ? __ATOMIC_RELAXED
                                                                 : static_cast<int>(order);
        return __atomic_compare_exchange_n(&value, &expected, desired, false, static_cast<int>(order),
                                           failure);
    }

private:
    T& value;
};
#endif
using Vector = std::array<std::uint64_t, 2>;

class ExclusiveMonitor {
public:
    /// @param processor_count Maximum number of processors using this global
    ///                        exclusive monitor. Each processor must have a
    ///                        unique id.
    explicit ExclusiveMonitor(size_t processor_count);

    size_t GetProcessorCount() const;

    /// Marks a region containing [address, address+size) to be exclusive to
    /// processor processor_id.
    template<typename T, typename Function>
    T ReadAndMark(size_t processor_id, VAddr address, Function op) {
        static_assert(std::is_trivially_copyable_v<T>);
        const VAddr masked_address = address & RESERVATION_GRANULE_MASK;

#ifdef ARCHITECTURE_arm64
        AtomicRef<VAddr>{exclusive_addresses[processor_id]}.store(masked_address, std::memory_order_release);
        const T value = op();
        std::memcpy(exclusive_values[processor_id].data(), &value, sizeof(T));
#else
        Lock();
        exclusive_addresses[processor_id] = masked_address;
        const T value = op();
        std::memcpy(exclusive_values[processor_id].data(), &value, sizeof(T));
        Unlock();
#endif
        return value;
    }

    /// Checks to see if processor processor_id has exclusive access to the
    /// specified region. If it does, executes the operation then clears
    /// the exclusive state for processors if their exclusive region(s)
    /// contain [address, address+size).
    template<typename T, typename Function>
        requires std::is_trivially_copyable_v<T>
    bool DoExclusiveOperation(size_t processor_id, VAddr address, Function op) {
        if (!CheckAndClear(processor_id, address)) {
            return false;
        }

        T saved_value;
        std::memcpy(&saved_value, exclusive_values[processor_id].data(), sizeof(T));
        const bool result = op(saved_value);

#ifndef ARCHITECTURE_arm64
        Unlock();
#endif
        return result;
    }

    /// Unmark everything.
    void Clear();
    /// Unmark processor id
    void ClearProcessor(size_t processor_id);

private:
    bool CheckAndClear(size_t processor_id, VAddr address);

    void Lock();
    void Unlock();

    friend volatile int* GetExclusiveMonitorLockPointer(ExclusiveMonitor*);
    friend size_t GetExclusiveMonitorProcessorCount(ExclusiveMonitor*);
    friend VAddr* GetExclusiveMonitorAddressPointer(ExclusiveMonitor*, size_t index);
    friend Vector* GetExclusiveMonitorValuePointer(ExclusiveMonitor*, size_t index);

    static constexpr VAddr RESERVATION_GRANULE_MASK = 0xFFFF'FFFF'FFFF'FFFFull;
    static constexpr VAddr INVALID_EXCLUSIVE_ADDRESS = 0xDEAD'DEAD'DEAD'DEADull;
    static constexpr size_t MAX_NUM_CPU_CORES = 6; // Sync with src/core/hardware_properties
    boost::container::static_vector<VAddr, MAX_NUM_CPU_CORES> exclusive_addresses;
    boost::container::static_vector<Vector, MAX_NUM_CPU_CORES> exclusive_values;
    SpinLock lock;
};

}  // namespace Dynarmic
