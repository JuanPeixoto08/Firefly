<!--
# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->

# Firefly

A very experimental Nintendo Switch 2 emulator, focused on getting The Legend of Zelda: Ocarina of Time (2026) to run well. Firefly is a fork of [Lichii](https://github.com/zortos293/Lichii), which builds on [Eglantine](https://github.com/Djihads80/Eglantine), [Eden](https://git.eden-emu.dev/eden-emu/eden) and yuzu.

Firefly is an independent project. Please don't report problems with these builds to Lichii, Eglantine or Eden.

## What it adds to Lichii

- **Vegetation:** grass, trees and bushes draw again. The GPU culling shaders read their data through pointers that the storage buffer pass now follows. These commits are by Duck912 and Jonas, from Lichii's `port/ducky-oot-build16` branch.
- **Shaders:** the compute programs Ocarina of Time still rejected now translate: indexed `ULDC` widths, `ULEA` and `UIADD3` carries in any `UPn`, two-source `F2IP`, and uniform `I2F` byte selectors.
- **Fewer white flashes:** from Lichii pull request 121, by Kajcsi.
- **Debug server input:** the debug server can press buttons and move the sticks, so a test can reach a scene without anyone at the controls.

Most of this is meant to go back to Lichii as pull requests.

## Status

Tested only on Windows, with an RTX 5070 Ti, in Kokiri Forest. Expect visual bugs, crashes and slow areas elsewhere.

For better speed in Ocarina of Time, set **Graphics > Advanced > GPU accuracy** to **Low** for that game. It lets the GPU work run on two threads, which took frames from about 27 ms to 22 ms in testing.

## Requirements

You need your own console, and dumps of your own game, keys and firmware. Firefly includes none of them and won't help you get them.

## Building

The build steps are the same as Lichii's; see [docs/Build.md](docs/Build.md). On Windows with MSVC in a language other than English, run `chcp 65001` before building, so Ninja can read `cl`'s include notes.

## License

GPL-3.0-or-later, as the projects it comes from. Each source file keeps its own copyright notices. See [LICENSE.txt](LICENSE.txt) and [LICENSES](LICENSES).

Nintendo Switch and The Legend of Zelda are trademarks of Nintendo. Firefly isn't affiliated with Nintendo or with the projects above.
