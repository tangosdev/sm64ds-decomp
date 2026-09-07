# Getting mwccarm + NITRO-SDK

Two separate things, and **the pinned compiler is not one of them**. Read §1 first: it is
the one the build actually honours, and it is fully scripted. The Discord zip in §2 is for
the version *sweep*, which is iteration, not a verdict.

## 1. The pinned compiler: mwccarm 2004/b56 — scripted, no Discord needed

The build pins **`2004/b56`** (`rombuild.VERSION`, and `match.py`'s `CANONICAL`). That
binary is **not in `mwccarm.zip`** — no decomp collection carries it; they all floor at the
1.2 release, build 72, sixteen builds above it. Extracting the zip and stopping there
leaves you unable to verify anything, because a hit under any other version is iteration
and never the verdict (runbook §5).

It is recovered from public archives, in-process, with nothing executed:

```sh
pip install py7zr pefile
python tools/recover_cw2004.py          # -> tools/mwccarm/2004/b56/mwccarm.exe
```

The script range-fetches only the solid block it needs out of a 5.5 GB archive.org item,
splits the InstallShield payload, carves `mwccarm.exe`, and **verifies it by size and
SHA-256** before writing. It then borrows the three runtime DLLs from
`tools/mwccarm/1.2/sp2p3`, so **do §2 first** — the recovery depends on that trio being
present.

Notes from running it (recovered and verified 2026-08-05):

- **It transfers about 2 GB, not the "~24 MB" the script's own docstring claims.** The
  three header range-requests are tiny, but the solid block holding the installer is
  ~1,982 MB. Budget for that on a metered connection. Total runtime was a few minutes.
- **archive.org rate-limits.** A range request can come back HTTP ≥400 and the script
  exits with `curl` status 22. It is transient — the same range succeeded on retry — and
  completed pieces are cached in `.cw2004work/`, so simply re-run; an interrupted big
  block is not paid for twice.
- **Confirm it end to end, not just by hash.** The script checks SHA-256 and the
  `-version` banner (`Version 2.0 build 56`). To prove the result actually reproduces ROM
  bytes, verify one already-matched function:

  ```sh
  python - <<'PY'
  import sys; sys.path.insert(0, "tools"); import build_pin as BP
  print(BP.verify("src/engine/fader/_ZN5Fader13AdvanceInterpEv.cpp",
                  "_ZN5Fader13AdvanceInterpEv", 0x020175e8, 0x28, "arm9"))
  PY
  # -> (True, '2004/b56')
  ```
- **`license.dat` is not required.** The tooling points `LM_LICENSE_FILE` at
  `tools/mwccarm/license.dat`, but these builds run without it and the file is absent from
  a normal setup. Do not go hunting for it.
- `b56` **ships no linker.** The ROM linker stays `1.2/sp2p3` `mwldarm` (`LD_VERSION`).

Why this build: it materializes the address of a member read-modify-write where every
2005+ build folds it into the addressing mode — the long-standing "materialization wall".
See `notes/mwccarm-codegen.md` §6ah for what it wins and what it costs.

## 2. The version sweep: mwccarm.zip — still human-gated

`mwccarm` is proprietary but free, and the collection is not downloadable directly; it
lives in the decomp community's Discord.

- **DS Decompilation Discord**: https://discord.com/invite/gwN6M3HQrA — `mwccarm.zip` is in
  the **resources** channel. The invite is also linked from the dsd toolkit's community
  section (https://github.com/AetiasHax/ds-decomp), and the pret servers
  (pokediamond/pokeheartgold) pin the same files.
- Extract to `tools/mwccarm/`, so paths read `tools/mwccarm/2.0/sp2p2/mwccarm.exe`. It
  contains `1.2/` and `2.0/` trees with service-pack subdirs, each holding `mwccarm.exe`,
  `mwldarm.exe`, `mwasmarm.exe`.
- `mwccarm.exe` is a native Windows binary — no Wine on Windows.
- A **NITRO-SDK** archive (e.g. `NitroSDK-*.7z`) if pinned, for SDK headers/objects later.
  Not needed to start matching game functions.

That gives 24 of the 25 versions in `match.SWEEP`. The 25th is §1.

## 3. Which version do we need? — settled

This section used to say "unknown until we test", and cited the dead matty45 repo's guess
of `2.0/sp2p2`. Both are history. The answer is pinned:

| role | version |
|---|---|
| matching / build compiler | `2004/b56` |
| ROM linker (`mwldarm`) | `1.2/sp2p3` |
| sweep | all 25, iteration only |

Flags live in `rombuild.CFLAGS`; `CONTRIBUTING.md` quotes the matching set. Per-symbol
overrides for functions that need a different compiler live in
`config/rombuild-versions.txt`.

## 4. .gitignore

`tools/mwccarm/` must never be committed — proprietary, not redistributable. `tools/bin/`
(where `dsd` goes) and `.cw2004work/` are ignored too.
