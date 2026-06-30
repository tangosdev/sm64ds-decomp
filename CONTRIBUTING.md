# Contributing to sm64ds-decomp

Thanks for wanting to help. This is a from-scratch **matching** decompilation of
Super Mario 64 DS: every function we land is hand-written C that compiles to bytes
**identical** to the retail ROM. There are ~11,390 functions to go, so every matched
one is real progress.

## Get in touch

- **Discord: `beansntoast`**, DM me. Best for claiming work, asking which version/flags
  to use, or getting unstuck on a function.
- Or open a GitHub issue to claim a function/area so two people don't grind the same one.

## What you need

You bring your own copy of the game. Nothing copyrighted lives in this repo.

1. **Your own SM64DS cartridge dump (`.nds`).** The symbols and addresses here are
   verified against the **EU (Europe) retail build**, so use that build. The ROM and
   anything extracted from it are git-ignored and must never be committed.
2. **mwccarm** (Metrowerks CodeWarrior ARM). Proprietary but free; it's pinned in
   the DS-decompilation Discord, not downloadable directly. Full instructions are
   in [`notes/setup-mwccarm.md`](notes/setup-mwccarm.md). Extract to
   `tools/mwccarm/` (git-ignored).
3. **dsd** (the ds-decomp toolkit): https://github.com/AetiasHax/ds-decomp, it drives
   the analysis config in `config/` and rebuilds the ROM from objects.
4. **Python 3** plus a few packages:
   ```
   pip install ndspy capstone pyelftools
   ```

## First-time setup

```
git clone https://github.com/bmanus2-dotcom/sm64ds-decomp
cd sm64ds-decomp
pip install ndspy capstone pyelftools

# get mwccarm per notes/setup-mwccarm.md, then:
python tools/unpack.py "path/to/your-own-sm64ds.nds"   # -> populates extracted/ (git-ignored)
```

## The matching loop

The pinned compiler is **mwccarm `1.2/sp2p3`** (the 1.2 `base`/`sp2`/`sp2p3` trio is
codegen-identical) with flags:

```
-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
```

1. **Pick an unmatched function.** Ask in Discord or claim it on an issue.
2. **Disassemble it** to see what you're matching:
   ```
   python tools/disasm.py extracted/arm9.bin --offset 0x... --length 0x...
   ```
3. **Write C** for it (scratch first is fine).
4. **Compile + byte-diff**, relocation-aware:
   ```
   python tools/match.py --c yourfile.c --func name --addr 0x020xxxxx --size 0x.. \
       --version 1.2/sp2p3
   ```
   A match means every instruction word and every relocation slot lines up. Iterate
   until the bytes are identical.
5. **Promote** the matched C into `src/`, commit, and open a PR.

For a batch of banked matches, also run `python tools/linkcheck.py` before or after
opening the PR when you have the local progress files available. The normal match gate
compares unlinked objects and must wildcard relocation slots; `linkcheck` reconstructs
the linked bytes and catches wrong relocation destinations that would otherwise hide in
those slots. See [`notes/link-verification.md`](notes/link-verification.md).

Before you start writing C, skim [`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md) - it
documents how this exact compiler turns C into bytes (struct copies, bitfield shifts, C++
virtual/PMF dispatch, the register-allocation wall, and the common idiom families). Writing
with those habits in mind gets your first draft close and cuts iterations.

## Ground rules

- **Never commit copyrighted material.** No ROM, no extracted assets, no `mwccarm`.
  The `.gitignore` already enforces this, don't override it.
- **Import knowledge, write code.** You may use community symbol names and struct/field
  offsets (see [`CREDITS.md`](CREDITS.md)), but all C in `src/` must be hand-written from
  scratch against your own ROM. Do **not** paste another project's source.
- **Match to the byte.** A function counts only when its compiled bytes equal the ROM's.
- **Stay on the pinned toolchain.** Use the project's mwccarm version and flags so
  everyone's output is comparable.

## Submitting a PR

- Fork, branch, and PR against `main`.
- One function (or a small related cluster) per PR is ideal, easy to review.
- In the PR, note the mwccarm version/flags you matched with and the function's address.

If anything here is unclear or out of date, ping me on Discord (`beansntoast`), I'd
rather fix the docs than have you stuck.
