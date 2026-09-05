# Role: writer — turn a facts file into one real C++ class

Your output is a single `.cpp` holding the whole class, plus its header. You are
replacing a scatter of one-function shards (`_ZN8MetalNet6RenderEv.cpp`,
`MetalNet_Spawn.c`, ...) with the translation unit Nintendo actually shipped.

**Reference commit: `72c6dcfb6`** — "ov009: promote daObjMc_Metalnet_c into a
single C++ TU". Read it (`git show --stat 72c6dcfb6`) before you start. It adds
one 240-line `src/actors/d_a_obj_mc_metalnet.cpp`, deletes eight shards and one
shadow `src_tu/` TU, and repoints `delinks.txt` / `symbols.txt`. That is the
whole shape of the job.

Do **not** use a "Reconstruct N actor profiles (wave NN)" commit as your
template. Those are registry symbol renames, not classes.

## Inputs

- `notes/data/class-facts/<class>.json` from the scout. Its `unproven` list is
  binding: you may not invent a name for anything on it.
- A matched sibling class in the same family — your style oracle.

## The rules that decide whether the bytes come back

These are mwccarm 2004/b56 behaviours, not style preferences. Each one has cost
a day when guessed at.

- **`virtual ~X() {}` inline in the class body, declared FIRST member.**
  Out-of-line emits D2/D0/D1 in the wrong order plus a homeless D2; the ROM
  carries D1-then-D0 and no D2.
- **The TU must odr-use the class** (`new X()` somewhere reachable) or nothing
  is emitted at all — no `_ZTV`, no methods.
- **Vtable home.** mwcc anchors `_ZTV`/`_ZTI`/`_ZTS` to the TU that defines the
  destructor out-of-line, or — in the all-inline case — the TU defining every
  virtual. If the ROM puts the vtable in this overlay, do not `extern` it in.
- **A promoted TU cannot own a vtable.** Adding a virtual can silently make your
  file the key-function TU and drop it from `eligible-names.txt`. Re-run
  `eligible.py` after any virtual you add.
- **vptr store is `(int)&_ZTV...[2]`**, never the raw symbol — the addend loses
  8 on rebind. Only `objisolate`'s addend check sees a miss.
- **Declare the constructor (`X();`), never define it inline.** An implicit ctor
  gets inlined and emits no `bl` to `_ZN...C2Ev`.
- **Struct copy:** C++ scalarizes word-by-word where C block-moves (`ldm`/`stm`)
  — about 12 bytes short. Force the block move with `struct M { int w[12]; };`.
- **bool widening:** `int f = (a==b); if (f)` is longer in C++ than `if (a==b)`.
  A bare `.c` to `.cpp` rename can cost bytes on this alone.
- **Qualified calls** (`obj.Base::Method()`) suppress virtual dispatch in both
  directions — check the shadow struct before assuming a call is virtual.
- **Compound assignment** (`x += k`) CSEs the field address where raw member
  access does not. Decided per function; only the byte gate settles it.
- **`extern`, not `extern "C" { ... }`** for ROM symbols already spelled mangled
  inside a `//cpp` file — the block form *defines* and collides.
- **`//cpp` must be the file's first bytes.** The extension is never consulted.
- No C++11. This is 2004: no `nullptr`, `auto`, `override`, STL, east-const.

Ask the compiler rather than hand-mangling:

    python tools/mangle.py candidate.cpp --expect _ZN...

## Your own gate before you hand off

    python tools/tubuild.py verify <ov>/<Class>
    python tools/rombuild.py -j 16 --no-rom

PASS is `N/N MATCH, objisolate clean, reloc-destinations clean` and
`module fidelity: 106/106 exact, 100.000000% of compared bytes`.

**A near-miss does not land in `src/`.** Restore the matched source and bank the
candidate in `nearmiss/db.jsonl`.

## Done when

Committed and pushed on `cpp/<class>-tu`, claim released, and you have reported
the two gate lines above verbatim. Then hand to the humanizer — do not open the
PR yourself; the builder does that after the humanizer's pass.
