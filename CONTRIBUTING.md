# Contributing to sm64ds-decomp

Thanks for wanting to help. This is a decompilation of Super Mario 64 DS
written as a love letter to the original EAD team: every file in `src/`
should read like idiomatic, period-accurate C++ from 2004 *and* compile
byte-identical to the retail ROM under the pinned compiler. See
[`AGENTS.md`](AGENTS.md) for what a change looks like and what the merge gate
checks — this file is setup and ground rules. For deeper reference material
(matching craft, C++ conversion, provenance), see
[`notes/README.md`](notes/README.md).

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
[`notes/codegen-rules.md`](notes/codegen-rules.md) — it documents how this
exact compiler turns C++ into bytes (struct copies, vtable/PMF dispatch,
destructor-variant order, the register-allocation wall). Writing with those
habits in mind gets a first draft much closer. The condensed rules live there;
the dated discovery log they're mined from is `notes/mwccarm-codegen.md`.

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

Coordination runs through a lock service at tangos.dev. The scheduler already
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

If anything here is unclear or out of date, ping me on Discord
(`beansntoast`) — I'd rather fix the docs than have you stuck.
