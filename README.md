<!--
# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->

<p align="center">
  <img src="dist/branding/firefly_icon.png" alt="Firefly" width="160">
</p>

# Firefly

Firefly is a very experimental emulator for a current-generation hybrid game console. It is tuned to run one game as well as possible, so expect other software to run worse, or not at all.

Firefly is a fork of [Lichii](https://github.com/zortos293/Lichii), which builds on [Eglantine](https://github.com/Djihads80/Eglantine), [Eden](https://git.eden-emu.dev/eden-emu/eden) and yuzu. It is an independent project: please don't report problems with Firefly builds to any of them.

## What Firefly shares, and what it doesn't

Firefly is only the emulator: its source code and the builds made from it.

- **No games, keys, firmware or system files** come with Firefly, and none will be shared or linked here.
- **No copyrighted material** of any console maker or game publisher is included.
- **To use it, you need your own console** and dumps you made yourself, from hardware and games you own.
- Requests for games, keys or firmware will be closed without a reply.

Firefly isn't affiliated with, endorsed by or sponsored by any console maker or game publisher. All trademarks belong to their owners.

## What Firefly changes

Firefly starts from Lichii's `nx2` branch, kept up to date, and adds:

- **Far fewer white flashes.** Vegetation, or the whole scene, used to flash white for a frame, mostly when frames ran slow. Native compute programs give their storage buffers no size, so each dispatch marked up to 8 MiB as written by the GPU, and the CPU's later writes to those pages waited for the next fence. Firefly marks only as far as each shader's stores can reach, worked out from the shader itself (daniel-lynch's store-extent analysis). With the emulator held to three CPU cores, a 40-second test went from 85-93 flashes to none.
- **GPU accuracy Low by default for the tested game.** That setting lets the GPU work run on two threads; at High the game runs at about 25-30 FPS instead of 60. The game starts at Low unless you choose an accuracy in its own settings.
- **Low-end mode** (Graphics > Advanced, per game, off by default). The game's upscaling network is emulated in software, which costs weak GPUs most of the frame; low-end mode draws a plain upscale in its place and lets the game lower its own resolution.
- **Less busy waiting on small CPUs.** Idle emulated cores poll for work only when the host has a CPU to spare for each of them.
- **A faster check for continuous GPU memory.** On every bind, native storage buffers ask how far a GPU address stays continuous in device memory. Lichii keeps 64 recent answers per thread. The tested game binds more distinct ranges than that, which caused about 29,000 cache misses a second. Each miss walked up to 8 MiB of page tables, and together they took 6% of the GPU back thread. A range can only break at a page boundary, so Firefly keeps one answer per page, which covers every address and size inside it. A cross-check against the full walk over 11.5 million calls found no difference.
- **Input through the debug server.** The opt-in debug server can press buttons and move the sticks, so a test can reach a scene and measure it without anyone at the controls.
- **Fixes from other forks:** from Eglantine, the Vulkan scheduler kept to one thread while the second GPU thread submits, upload scans skipped for buffers the game hasn't written, and smaller texture descriptor tables; from open Lichii pull requests, the upscaler stand-in (#143) and GPU time scaling (#114).
- **Its own name, icon and update channel.** It uses the same data folder as Lichii, so settings, saves and shader caches carry over.

Fixes that also help Lichii are sent back there as pull requests. Several of the fixes the tested game needed already reached Lichii that way; see [Credits](#credits).

## Performance

Measured at the same spot in the tested game's first area, at 1x resolution, with GPU accuracy set to Low for that game:

| Build | Frame time | Frame rate |
|---|---|---|
| Lichii, before its early November 2026 fixes | about 22 ms | about 45 FPS |
| Lichii `nx2` with those fixes | 17.0 ms | about 58 FPS |
| Firefly | 16.7 ms, with 5 to 10 ms to spare before each vblank | 60 FPS (the game's cap) |

Test machine: Windows 11, AMD Ryzen 5 9600X, NVIDIA GeForce RTX 5070 Ti, 32 GB RAM. The numbers come from that one spot, so the same scene can be compared between builds. Beyond it, the game has been played for longer stretches on the same machine and runs relatively well.

## Recommended settings

- **GPU accuracy: Low.** Firefly picks it for the game by itself. To choose another, right-click the game, choose **Configure Game**, then **Graphics > Advanced**.
- **Low-end mode** on weaker PCs, in the same place. The image is softer, but the game runs much lighter on the GPU.
- Keep **Use persistent pipeline cache** on. New areas still stutter the first time their shaders compile.

## Status

- **Platforms:** tested only on Windows with an NVIDIA GPU. Other builds may compile, but nobody has tested them.
- **Known problems:** short hitches the first time an area loads, while its shaders compile; an occasional white flash; frame rate dips in the busiest areas. Expect some visual bugs: it is still an experimental emulator.

## Downloads

Builds are on the [Releases](https://github.com/Kradnah/Firefly/releases) page, which Firefly also checks for updates.

## Reporting problems

Open an issue on this repository with:

- what you did and what went wrong, with a screenshot for visual bugs;
- your CPU, GPU and driver version;
- the log, `%APPDATA%\eden\log\eden_log.txt` (and `eden_log.txt.old` if the problem happened before the last restart).

Don't attach game files, keys or firmware to issues.

## Building

The build steps are the same as Lichii's; see [docs/Build.md](docs/Build.md). On Windows with MSVC in a language other than English, run `chcp 65001` before building so that Ninja can read `cl`'s include notes.

## Credits

Firefly is a small layer on years of other people's work.

- **yuzu** and everyone who wrote it, for the emulator everything here grew from.
- **[Eden](https://git.eden-emu.dev/eden-emu/eden)**, for keeping that work going.
- **[Eglantine](https://github.com/Djihads80/Eglantine)**, by Djihads80, for support for the newer console.
- **[Lichii](https://github.com/zortos293/Lichii)**, by zortos293, and its contributors. Firefly relies on these Lichii changes in particular:
  - **Duck912** and **Jonas**: the pointer and storage buffer work that made the game's vegetation draw, ported to Lichii by zortos293.
  - **Kajcsi**: the occlusion query fixes behind the white flashes, skipping the upload of render targets that a clear overwrites, and early wakes in timed waits.
  - **nyakuoff**: the first cache of continuous GPU ranges, which Firefly's per-page cache builds on.
  - **Zarigis**: buffer cache uploads and downloads of GPU-written pages.
  - **Shrxmy**, **Felipe Lima**, **Inc0rporate**, **Kompas9**, **resadent** and **cr1ckt**: shader, Vulkan, video, boot and build fixes.
- **daniel-lynch**: the store-extent analysis behind the white flash fix, from his Lichii fork.
- **rdenno**, **Mvii7** and **nowhere02** (Eglantine): the scheduler, buffer scan and descriptor table changes.
- **Kradnah**: Firefly itself. Kradnah chooses what to fix, tests every build in the game and decides what goes in, including the compute shader lowering the tested game needed (also merged into Lichii). The debugging, profiling and code were done together with Claude, Anthropic's AI assistant.

If your work is in Firefly and missing here, open an issue and it will be added.

## License

GPL-3.0-or-later, like the projects it comes from. Each source file keeps its own copyright notices. See [LICENSE.txt](LICENSE.txt) and [LICENSES](LICENSES).

## Supporting Firefly

Firefly is free and will stay free. Donations are optional, pay only for Firefly's own development (test hardware and time), and don't unlock anything.

Donations are taken in crypto. **Send only the coin listed, on the network listed**: anything sent on another network is lost.

| Coin | Network | Address |
|---|---|---|
| USDT | TRON (TRC20) | `TMskyYQWTsBmB4FJ63yVnhrhRP5vPoiyoE` |
| BTC | Bitcoin (Taproot) | `bc1pe9rjdh94kgu53g8gg94upws60lpx0mmsqmhchhn08au4zd37wnqqvnzp0s` |
| ETH | Ethereum (ERC20) | `0x8299B43d4B784EB78DA593BCc033871F970252f6` |
| SOL | Solana | `9QD6s9hyqK6KdD64F9vucxkB3znJwDuVBseqS1gQn5K8` |

Copy the address rather than typing it, and check the first and last characters before you send.

Please also support the projects Firefly builds on, through the links on their own pages.
