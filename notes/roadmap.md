# Roadmap: from ROM to a native PC EXE

**North Star:** a Windows `.exe` that runs Super Mario 64 DS natively on PC, no
emulator, built entirely from source. This is exactly what happened to the N64 game
(`sm64-port`), and it is reachable, but only as the far end of a long road. Four
phases: **MATCH -> LIFT -> PORT -> POLISH**. Each has a crisp gate that says it is done.

Current position: **Phase 1, 65.9%** (7,504 / 11,390 functions).

---

## Phase 1 - MATCH  [ACTIVE - 65.9%]
Rebuild the retail `.nds` byte-for-byte from hand-written C compiled with mwccarm.
Matching is what makes the C *provably* the original logic, not a guess. This is the
slow, rigorous, multi-year bulk of the whole project. It runs on three tracks:

- **Templates** - a rule engine recognizes common function shapes (getters, setters,
  bitfields, struct copies, ctors) and generates the C for free. Clears the easy bulk.
- **AI crack-loop** - a fan-out of Claude agents cracks the mid-band functions, each
  attempt verified against the ROM byte-for-byte before it counts. This is the engine
  that carried the project from ~11% to 66%, and it is what backing pays for.
- **Hand-match** - the hard residue: genuine one-off logic and the codegen-floor
  functions no automated pass can reach, matched by hand one at a time.

**Done when:** `dsd rom build` turns the source back into the exact retail ROM,
every function, zero diffs.

## Phase 2 - LIFT  [0%]
A byte-matching function is correct but still bent to mwccarm's quirks. For a port,
the engine and game logic have to become clean, portable C, and every place the code
pokes DS hardware has to be found and walled off behind an abstraction layer:
3D (GX geometry engine), 2D engine, SPU audio, touch/input, NARC filesystem.

**Done when:** the engine compiles and runs off-DS against a hardware interface,
with no direct DS register access left in the game logic.

## Phase 3 - PORT  [0%]  (this is the phase that produces the EXE)
Replace the DS hardware layer with PC equivalents and compile for x86_64-windows:

- **Graphics:** reimplement the DS GX pipeline over OpenGL / a custom renderer. The
  DS 3D pipeline is unusual, so this is the bulk of the porting work.
- **Audio:** SDAT / SSEQ playback over SDL or a custom mixer.
- **Input:** keyboard and controller into the game's input layer.
- **Assets:** loaded at runtime from files the user extracts from their own ROM.

**Done when:** `sm64ds.exe` boots and plays start-to-finish on PC, no emulator,
linking SDL2 + a GL renderer.

## Phase 4 - POLISH  [0%]
Everything the N64 PC port added once the base port worked: widescreen, high-res,
uncapped framerate, remappable controls, and a foundation for mods.

**Done when:** it is a better way to play the game than the original hardware.

---

## Honest notes
- The N64 PC port appeared AFTER its matching decomp was essentially complete (2019).
  We are still inside Phase 1. Every matched function is a brick in this road; the EXE
  is the far end of it, not the next step.
- What the community shares is the open-source **code**, which each person builds
  themselves against their own cartridge dump. Handing around a compiled
  Nintendo-game EXE (or a ROM, or assets) reliably draws a Nintendo DMCA - that is what
  keeps happening to these ports. So the win condition is a great public repo people
  can build from their own ROM, not a binary passed around. Same result for an owner,
  very different legal exposure.
