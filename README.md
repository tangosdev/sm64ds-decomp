# sm64ds-decomp

A from-scratch effort to decompile **Super Mario 64 DS** into matching C.

## Progress

```
Functions  ███████████░░░░░░░░░░░░░░░░░░░  38.0%   4,328 / 11,390
Code size  ███░░░░░░░░░░░░░░░░░░░░░░░░░░░  11.7%   260,388 / 2,234,028 bytes
```

Every arm-mode function in the game, drawn as a treemap. Each rectangle is one
function sized by its byte count, green for matched and gray for unmatched, grouped
by module.

![Decompilation progress treemap](docs/progress-treemap.svg)

For an interactive version where you can hover any function for its name, address,
size, and status, see the [progress treemap on GitHub Pages](https://bmanus2-dotcom.github.io/sm64ds-decomp/).

## What "matching" means

The goal is source code that, when compiled with the original toolchain, produces a
binary byte-for-byte identical to the retail ROM. This is the same standard the N64
`sm64` project holds to. Every matched function is checked against the ROM, so the
source is known to be correct.

## Legal and scope

This repo contains only original work: the tooling, the hand-written C, and the notes.
It contains no ROM and no extracted Nintendo assets. Those are read locally from a
cartridge dump you own, and they are git-ignored. Do not commit anything derived from
the ROM's data or assets.

## Notes on the numbers

Function count climbs faster than code size because the small, regular functions are
matched first, while most of the remaining bytes live in the large, call-heavy
functions. The original compiler has been pinned to **mwccarm 1.2/sp2p3** with these
flags:

```
-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
```

## How matching works

Every candidate is verified the same way: compile it with mwccarm, then compare the
result to the ROM byte-for-byte, relocation-aware (call and data references are slots
the linker fills in, so they are compared structurally). Nothing counts as matched
until that check passes. The work is organized in tiers so the automatic methods clear
as much as possible before any manual effort:

1. **Automatic templates.** A set of rules recognizes common function shapes (constant
   returns, field getters and setters, bitfield reads, struct copies, simple wrappers,
   constructors, and destructors), generates the C, and confirms it against the ROM.
   This clears the bulk of small, regular functions with no hand work.
2. **Hand-written.** For functions with real logic, you write the C yourself and verify
   each attempt until it is byte-identical. A decompiler such as Ghidra is useful for
   reading the function, though its output never matches on its own.

## Setup

You supply your own cartridge dump. Full setup (Python dependencies, the proprietary
mwccarm compiler from the DS-decomp Discord, the dsd toolkit, and unpacking your ROM)
is in [CONTRIBUTING.md](CONTRIBUTING.md) and
[notes/setup-mwccarm.md](notes/setup-mwccarm.md).

Short version:

```
pip install ndspy capstone pyelftools
# get mwccarm + license.dat per notes/setup-mwccarm.md, then:
python tools/unpack.py "path/to/your-own-sm64ds.nds"
```

## How you can help

Every matched function moves the project forward, and the automatic tier means even a
small amount of time goes a long way. See [CONTRIBUTING.md](CONTRIBUTING.md) for the
full workflow.

**Contributing code.** Pick a function, write C for it, verify it compiles to the same
bytes as the ROM, then open a pull request. One function or a small related group per PR
is ideal. Use only your own legally dumped ROM, and never commit it.

**Coordination.** Discord `beansntoast` for questions and claiming work, or open a GitHub
issue so two people do not grind the same function.

**Funding.** Sponsoring helps cover both the development time and the AI compute that the
matching runs on. Most functions are matched for free by the automatic templates, and the
harder ones are matched with AI assistance at roughly 5 to 10 cents of usage each. Spread
across everything matched so far, that works out to about 5 cents per function on average,
and reaching the current progress has cost on the order of $200 in AI usage. You can
sponsor at https://github.com/sponsors/bmanus2-dotcom. This goes toward development and
compute only; it has nothing to do with Nintendo's ROM or assets.

**Fastest way to start with an AI assistant.** Paste this into a Claude Code session and
it will pull the repo and set you up:

```
Clone https://github.com/bmanus2-dotcom/sm64ds-decomp and set up the Super Mario 64 DS
matching-decompilation toolchain on my machine. Do these in order:
1. Read CONTRIBUTING.md and notes/setup-mwccarm.md in the repo.
2. Install the Python dependencies: ndspy, capstone, pyelftools.
3. mwccarm and license.dat cannot be downloaded automatically: they are in the DS-decomp
   Discord (https://discord.com/invite/gwN6M3HQrA, resources channel, mwccarm.zip) and I
   have to fetch them by hand. Wait for me to do that, then help me place them under
   tools/mwccarm/ and set the LM_LICENSE_FILE environment variable.
4. Unpack my own SM64DS cartridge dump with tools/unpack.py. This writes the ARM9, ARM7,
   and overlay binaries into extracted/ (gitignored), including both the compressed
   arm9.bin and the decompressed arm9_dec.bin. Use the decompressed image for disassembly.
5. Confirm the toolchain runs: re-match a function we have already landed (any file in
   src/) with tools/match.py and check that it still reports identical bytes.
6. Before matching, read CLAIMS.md and pick a module or address range that nobody has
   claimed. Add a row claiming it (range, my handle, date), commit it on its own, and
   push, so no one else grinds the same functions. Work only inside that claimed range.
7. Pick an unmatched function from the claimed range, help me write matching C for it,
   and verify with tools/match.py that it compiles to the same bytes as the ROM.
Use only my own legally dumped ROM. Never commit the ROM or anything extracted from it.
```

## Credits

Symbol names and struct knowledge build on community reverse-engineering work. See
[CREDITS.md](CREDITS.md). The rule is import knowledge, write code: you may use known
symbol names and field offsets, but all C must be written from scratch against your own
ROM.

## License

The original work in this repo (the C, the tooling, the notes) is released under the MIT
License, see [LICENSE](LICENSE). This applies only to that original work and grants no
rights to any Nintendo material, which is not present here.
