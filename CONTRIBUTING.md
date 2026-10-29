# Contributing to sm64ds-decomp

Thanks for wanting to help. This is a decompilation of Super Mario 64 DS
written as a love letter to the original EAD team: every file in `src/`
should read like idiomatic, period-accurate C++ from 2004 *and* compile
byte-identical to the retail ROM under the pinned compiler. See
[`AGENTS.md`](AGENTS.md) for what a change looks like and what the merge gate
checks — this file is setup and ground rules.

For source conventions and the local/CI style check, read the
[canonical source-style guide](notes/source-style.md). It covers initialization,
naming, casts, formatting, and documented matching exceptions.

## What "matching" means

The goal is source code that, when compiled with the original toolchain,
produces a binary byte-for-byte identical to the retail ROM. This is the same
standard the N64 `sm64` project holds to. Every matched function is checked
against the ROM, so the source is known to be correct.

The matching compiler is pinned to **mwccarm 2004/b56** with these flags (the
1.2 `base`/`sp2`/`sp2p3` trio remains available for version sweeps; the linker
is still 1.2/sp2p3 `mwldarm`, because b56 ships no linker):

```sh
-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
```

C++ sources compile under the same pin with `-lang c++` in place of `-lang
c99`, plus `-Cpp_exceptions off`. `tools/match.py` makes both substitutions
itself for a file whose first line is `//cpp`.

## What counts as matched

A function counts as matched when the source in this repo, compiled with the
pinned compiler, produces exactly the bytes that are on the cartridge. Not
similar bytes. The same bytes. If the check fails, it does not count, however
close it looks.

Almost all of the game was written in C and C++, so almost all of that work
is writing C and C++. A small number of routines were neither. Nintendo wrote
those by hand in assembly, because they do jobs the C language has no way to
ask for: driving the chip's cache, calling into the DS firmware, switching
the processor between modes, and a few pieces of the compiler's own maths
library that hand back two answers at once.

There is no C to find for those. The honest source for them is the same
assembly, so that is what this repo carries, and each one is checked against
the cartridge exactly like everything else. They count as matched, and every
file counted that way carries a `HAND-ASM PRIMITIVE` line at the top so
nobody mistakes it for recovered C. The caption under the MATCHED bar says
how many there are.

That exception is narrow on purpose. A function only qualifies when its body
contains an instruction C cannot express at all. If it is ordinary code that
we simply cannot reproduce yet, writing it out as assembly proves nothing, so
it does not qualify and does not count. Those files are marked `NONMATCHING`
instead, and unmatched means just that: the original was C or C++, and we do
not yet have source the compiler turns into those exact bytes. The rule is
written up in [notes/asm-policy.md](notes/asm-policy.md).

## How matching works

Every candidate is verified the same way: compile it with mwccarm, then
compare the result to the ROM byte-for-byte, relocation-aware (call and data
references are slots the linker fills in, so they are compared
structurally). Nothing counts as matched until that check passes, including
the hand-written assembly primitives above: the banner explains why a file is
assembly, it never excuses it from the byte check.

1. **Automatic templates.** A set of rules recognizes common function shapes
   (constant returns, field getters and setters, bitfield reads, struct
   copies, simple wrappers, constructors, and destructors), generates the C,
   and confirms it against the ROM. This cleared the bulk of the small,
   regular functions with no hand work.
2. **Hand-written.** For functions with real logic, you write the source
   yourself and verify each attempt until it is byte-identical. A decompiler
   such as Ghidra is useful for reading the function, though its output never
   matches on its own.

Almost-right attempts are evidence, not trash: see [Easy
pickings](#easy-pickings-the-near-miss-database) below.

`tools/linkcheck.py` performs the stronger relocation destination check: it
reconstructs each function's linked bytes and compares them to the ROM,
catching wrong callees or globals that the normal unlinked byte diff would
wildcard. See [notes/link-verification.md](notes/link-verification.md).

Pull requests are then validated automatically. Each changed source file is
compiled and compared against ROM bytes on a build box, which catches wrong
relocation destinations and non-reproducing near misses before anything
lands. See [notes/pr-validation.md](notes/pr-validation.md). Class and TU
work coordinated through [the agent
protocol](notes/agents/README.md) also gets an independent source review in
the work queue before it lands.

## Get in touch

- **Discord: `beansntoast`**, DM me. Best for claiming work, asking which
  version/flags to use, or getting unstuck.
- Or open a GitHub issue to claim a function/class so two people don't grind
  the same one.

## The easy path: tangOS Console

[tangOS Console](https://github.com/tangosdev/tangOS) is the free desktop app
this project runs on. Install it, point it at your clone, and connect your AI
assistant (Claude, GPT, Grok, DeepSeek, or a local model) over MCP. It
coordinates claims, verifies every candidate against the real toolchain, keeps
`src/` on-convention, and formats your PR. Installer:
https://github.com/tangosdev/tangOS/releases/latest

The manual setup below still works and is what tangOS automates.

## What you need

You bring your own copy of the game. Nothing copyrighted lives in this repo.

1. **Your own SM64DS cartridge dump (`.nds`).** Symbols and addresses here are
   verified against the **EU (Europe) retail build** — use that build. The ROM
   and anything extracted from it are git-ignored and must never be committed.
2. **mwccarm** (Metrowerks CodeWarrior ARM). Two parts, both required — full
   instructions in [`notes/setup-mwccarm.md`](notes/setup-mwccarm.md):
   - the **sweep** (`mwccarm.zip`, 24 versions): proprietary but free, pinned
     in the DS-decompilation Discord, not downloadable directly. Extract to
     `tools/mwccarm/` (git-ignored).
   - the **pinned build `2004/b56`**, which the zip does not contain and which
     the build actually honors: `python tools/recover_cw2004.py` recovers and
     verifies it from public archives. Run it *after* the zip — it borrows
     that trio's runtime DLLs.
3. **dsd** (the ds-decomp toolkit): https://github.com/AetiasHax/ds-decomp —
   drives the analysis config in `config/` and rebuilds the ROM from objects.
   Grab the `0.11.0` binary for your platform and put it at
   `tools/bin/dsd.exe` (`tools/bin/` is git-ignored).
   `rombuild.py` and `tubuild.py linkcheck` check this version before preparing
   build inputs and record the executable's SHA-256 in their reports. Newer
   versions can change delinking and linker scripts; they need a separately
   validated toolchain migration.
4. **Python 3** plus a few packages:
   ```sh
   pip install ndspy capstone pyelftools    # core
   pip install py7zr pefile                 # only for tools/recover_cw2004.py
   ```

## First-time setup

```sh
git clone https://github.com/tangosdev/sm64ds-decomp
cd sm64ds-decomp
pip install ndspy capstone pyelftools py7zr pefile

# 1. get mwccarm.zip per notes/setup-mwccarm.md and extract to tools/mwccarm/
# 2. recover the pinned compiler the zip does not carry (verifies by SHA-256):
python tools/recover_cw2004.py                         # -> tools/mwccarm/2004/b56/
# 3. put the dsd 0.11.0 binary at tools/bin/dsd.exe
# 4. extract your own ROM:
python tools/unpack.py "path/to/your-own-sm64ds.nds"   # -> populates extracted/ (git-ignored)
```

Without step 2 nothing can be byte-verified: `rombuild.py` and
`build_pin.verify` only accept `2004/b56`, so a sweep hit under another
version is iteration, never a verdict. Confirm the whole chain works:

```sh
python tools/rombuild.py -j16 --no-rom
# module fidelity: 106/106 exact, 100.000000% of compared bytes
```

Before writing any code, skim
[`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md) — it documents how this
exact compiler turns C++ into bytes (struct copies, vtable/PMF dispatch,
destructor-variant order, the register-allocation wall). Writing with those
habits in mind gets a first draft much closer.

## Easy pickings: the near-miss database

[`nearmiss/db.jsonl`](nearmiss/db.jsonl) holds compiling candidates that are
close to matching but not exact — many are only a few instructions off:

```sh
python tools/nearmiss_db.py list --max-div 4
```

Take the stored `c_source` as a starting point and iterate with
`tools/fdiff.py` (prints exactly which instructions differ, relocation-aware).
Some residuals are a known compiler wall (instruction ordering,
base-address materialization — see `notes/mwccarm-codegen.md`); if a 1-2
instruction gap refuses to close, that may be why. Ask on Discord.

## Coordinating your work: get a claims key

For coordinated class/TU agents, use [the v2 protocol](notes/agents/README.md) and
[the promotion workflow](notes/tu-promotion-conventions.md). Resume existing work
and claims; a missing key for the separate service below does not bypass v2.

For standalone matching outside that fleet, coordination uses the lock service at tangos.dev. The scheduler already
**reads** it — it won't hand you work someone else holds — even with no key.
To **announce** the work you take, mint one:

1. Sign in at **https://tangos.dev/account** (Google or GitHub).
2. Click **"Mint a service token"** — copy it, it's shown once.
3. Save it to `tools/claims_key.txt` (gitignored), or export it as
   `CLAIMS_API_KEY`.

tangOS Console has this built in — the key button next to Settings mints and
stores it encrypted. Without a key you still get one-way protection, so it's
optional, but mint one if you're running batches.

## Ground rules

- **Never commit copyrighted material.** No ROM, no extracted assets, no
  `mwccarm`. `.gitignore` already enforces this — don't override it.
- **Import knowledge, write code.** You may use community symbol names and
  struct/field offsets (see [`CREDITS.md`](CREDITS.md)), but everything in
  `src/` must be hand-written from scratch against your own ROM. Do not paste
  another project's source.
- **Byte-identical or it doesn't count.** See `AGENTS.md` for the gate.
- **Stay on the pinned toolchain** so everyone's output is comparable.

## Submitting a PR

See [`AGENTS.md`](AGENTS.md) for what belongs in the PR and how it's
reviewed. Fork, branch, and PR against `main` — one function, class, or small
related cluster per PR is easiest to review.

## Funding

Sponsoring helps cover development time and the AI compute the work runs on.
Most functions were matched for free by the automatic templates, and the
harder ones with AI assistance at a few cents of usage each. You can sponsor
at https://github.com/sponsors/tangosdev or back the project on Patreon at
https://www.patreon.com/c/the_tango. This goes toward development and compute
only; it has nothing to do with Nintendo's ROM or assets.

If anything here is unclear or out of date, ping me on Discord
(`beansntoast`) — I'd rather fix the docs than have you stuck.
