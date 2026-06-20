# decomp-permuter integration

Wiring [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) to our mwccarm
toolchain. The permuter brute-forces the **exact-codegen** tail of matching (register
allocation, instruction ordering) by randomly applying semantics-preserving C
transformations and scoring each compile against the target. This is the free,
no-LLM way to clear the coloring wall that the fan-out floors at ~8% on.

The decomp-permuter `arm32_compile_example.sh` is itself written for **mwccarm on a DS
decomp project** (pokeheartgold) with nearly identical flags to ours, so this is a proven
fit, not an experiment.

## Split-the-problem model

- **Semantics** (what the function does -> logically-correct C): LLM fan-out / hand. Good.
- **Codegen** (exact regalloc + ordering): the permuter, for free. This is our 8% wall.

Stop paying LLM tokens to nail register allocation; let the permuter brute-force it.

## Status: WORKING (validated 2026-06-20)

End-to-end pipeline runs on our mwccarm toolchain, native Windows, no external objdump:
- [x] **Compiler wrapper** (`mwccarm_compile.sh`): compiles a candidate `.c` to `.o` with
  our canonical compiler+flags (1.2/sp2p3). Verified.
- [x] **Capstone scorer** (`cap_objdump.py`): a drop-in for the permuter's external objdump
  using capstone (which we already use). Handles mwccarm ELF candidates (extract `.text` +
  `.rel.text` relocs) and a raw-bytes target. Wired via `objdump_command` in settings.toml,
  so the permuter's stock `Scorer` (penalties regalloc=5, reorder=60, insert/delete=100) is
  used unchanged. **Validated: scores 0 for a true match, 35 for the r0/r1 regalloc distance
  on `func_020570c0`.**
- [x] **Runner**: a permuter dir (`compile.sh` -> wrapper, `target.o`, `base.c`,
  `settings.toml`) runs via `permuter.py <dir> --stop-on-zero -j N`. Confirmed it compiles,
  scores, and hill-climbs (35 -> 20 on the toy function; tiny leafs have little permutation
  surface -- real gains are on medium functions).
- [x] **Import helper** (`import_func.py`): given `--module/--addr` (or `--name`) and a
  `--base` seed, writes target.o (raw ROM bytes), target.o.relocs (offsets derived from the
  seed's own `.rel.text` -- authoritative, incl. data-pool relocs that config omits),
  compile.sh, base.c, and settings.toml. **PROVEN: cracked a real unmatched function** --
  `OAM::EnableSubOAM` (0x020219f0), base score 20 -> 0, the permuter found `G[0]=(long)0`
  (the cast shifts regalloc to match the ROM), independently oracle-verified and banked.
- [ ] **LLM <-> permuter loop** (next): run the permuter as a BACKGROUND job that only
  reports back on a perfect match (score 0). Do NOT feed half-mutated candidates to the
  LLM -- that causes the "doom loop / token burn" others have reported.

## Setup (reproducing on a fresh clone)

`vendor/` is gitignored, so re-cloning decomp-permuter needs these one-time steps:

1. `git clone https://github.com/simonlindholm/decomp-permuter vendor/decomp-permuter`
2. `python -m pip install toml pcpp pycparser capstone pyelftools`
3. Windows-compat patches to the cloned permuter (all small, marked
   `# Windows-compat (sm64ds-decomp)`):
   - `src/compiler.py`: route `compile.sh` through `bash` on Windows.
   - `src/main.py`: skip the Unix executable-bit check on Windows.
   - `src/preprocess.py`: use the in-process `pcpp` preprocessor instead of the external
     `cpp` binary (absent on Windows).
   - `src/objdump.py` (`get_arch`): default to ARM32 for a non-ELF file, so a raw-bytes
     `target.o` (ROM slice) is accepted.
4. Per-function: a dir with `compile.sh` (-> `mwccarm_compile.sh`), `target.o`, `base.c`,
   and `settings.toml` setting `compiler_type="mwcc"`, `func_name`, and
   `objdump_command = "python <repo>/tools/permuter/cap_objdump.py"`.

## Why this is the lever

The fan-out floors at ~8% on the hard residue because the LLM can't reliably reproduce
mwccarm's non-local register allocation. The permuter brute-forces exactly that, for free.
Best workflow: LLM/m2c writes logically-correct C (scores low but nonzero), permuter drives
it to a byte match. See the project memory `sm64ds-coloring` and `notes/mwccarm-codegen.md`.
