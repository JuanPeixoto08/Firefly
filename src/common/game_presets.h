// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/settings.h"

namespace Settings {

inline constexpr u64 MarioKartWorldProgramId = 0x0400C3F00006E000ULL;

inline void SetMarioKartWorldRecommendedSettings(Values& settings, bool global) {
    const auto session = [global](auto& setting, auto value) {
        setting.SetGlobal(global);
        setting.SetValue(value);
    };
    session(settings.renderer_backend, RendererBackend::Vulkan);
    session(settings.resolution_setup, ResolutionSetup::Res1X);
    session(settings.gpu_accuracy, GpuAccuracy::Low);
    session(settings.use_asynchronous_shaders, true);
    session(settings.astc_recompression, AstcRecompression::Bc3);
    session(settings.vram_usage_mode, VramUsageMode::Conservative);
    session(settings.use_reactive_flushing, false);
    session(settings.vsync_mode, VSyncMode::FifoRelaxed);
    session(settings.use_docked_mode, ConsoleMode::Handheld);
    session(settings.scaling_filter, ScalingFilter::ScaleForce);
    session(settings.anti_aliasing, AntiAliasing::Fxaa);
}

inline bool ApplyMarioKartWorldPreset(Values& settings, u64 program_id, bool accepted) {
    if (!accepted || program_id != MarioKartWorldProgramId) {
        return false;
    }
    SetMarioKartWorldRecommendedSettings(settings, false);
    return true;
}

inline constexpr u64 OcarinaOfTimeProgramId = 0x0400486024DEC000ULL;

// Firefly is tuned for Ocarina of Time at GPU accuracy Low, where the GPU work runs on two
// threads (High stops the split, about 25-30 FPS instead of 60). The game starts there for the
// session unless its own settings choose an accuracy; saved settings are not changed.
inline bool ApplyOcarinaOfTimeDefaults(Values& settings, u64 program_id) {
    if (program_id != OcarinaOfTimeProgramId || !settings.gpu_accuracy.UsingGlobal()) {
        return false;
    }
    settings.gpu_accuracy.SetGlobal(false);
    settings.gpu_accuracy.SetValue(GpuAccuracy::Low);
    return true;
}

} // namespace Settings
