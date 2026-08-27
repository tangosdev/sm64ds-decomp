# The CONVERTED tier measured the opposite of what it rewards

Measured 2026-08-16 on `bff018693`.

`tools/tiers.py` scores five criteria and publishes CONVERTED as the share of
`src/` files passing all five. It read **426 / 11,287 = 3.8%**, and the README
explained that as the tier being "by far the furthest behind" and one that "does
not move on its own."

That reading was wrong. A third of the work had been done and was not being
counted, because the `real_name` criterion asked a question no converted C++
method could answer.

## The defect

`real_name` was `not (PLACEHOLDER.match(sym) or MANGLED.match(sym))`, where `sym`
is the `// @symbol` marker or the filename stem — the **linker symbol**.

For a C++ method the linker symbol is necessarily mangled. `mwccarm` emits
`_ZN10KoopaShell13OnYoshiTryEatEv` for `KoopaShell::OnYoshiTryEat()`, and
`eligible.py` *requires* a file's defined symbol to equal its config symbol — it
rejects otherwise with `defines X, expected Y`, which is a live rejection in the
current tree:

```sh
1  defines daObjFl_Fall_Block_c_OnYoshiTryEat, expected func_ov022_021123d0
```

So a converted method cannot carry any other symbol, and the only way to pass
`real_name` was to **un-convert** the method back into a flat `extern "C"`
function. The tier paid you to undo the work it exists to measure.

This is not a hypothetical. `src/game/actors/KoopaShell/_ZN10KoopaShell13OnYoshiTryEatEv.cpp` is:

```cpp
// @symbol _ZN10KoopaShell13OnYoshiTryEatEv
#include "KoopaShell.h"
s32 KoopaShell::OnYoshiTryEat() { ... }
```

with a real `struct KoopaShell : Enemy` header behind it. It scored 4/5.

Two measurements pin it:

| | |
|---|---|
| CONVERTED files whose defined symbol is mangled | **0 / 426** |
| `src/` files with a mangled stem already defining a real `Class::Method` | **2,034** |
| ...whose *only* failing criterion was `real_name` | **1,022** |

Zero out of 426 is the whole finding: the exclusion was structural, not incidental.

A second, opposite defect lived in the same regex. `MANGLED = ^_Z[0-9NK]` does not
match `_ZT`, `_Zd` or `_Zn`, so thunks (`_ZThn80_N9ModelAnimD0Ev`) and the
`operator new`/`operator delete` pair scored as **real names**. The criterion
failed the files that had done the work and passed ones that had not.

## The fix

Ask the question of the name a *reader* sees, by demangling the file's own symbol
with `tools/demangle.py`, and reject any component that is still a placeholder.

```sh
real_name   972 -> 3,807
CONVERTED   426 (3.8%) -> 1,558 (13.8%)      +1,139 gained, -7 lost
```

The 7 losses are correct and are logged in
`config/converted-backslide-exceptions.jsonl`: six `_ZThn80_*` thunks and one
`FUN_0202a130.c`, all of which passed only through the `^_Z[0-9NK]` hole.

`PLACEHOLDER_PART` additionally rejects a placeholder wearing a real class prefix
— `Player::Unk_020c9e5c`, `CommonModel::Func_020160AC`, `ActorBase::Virtual38`.
The class half is genuine; the method half is the ROM address or a vtable slot
index, so the pair reads no better than the bare `func_020c9e5c`.

## The approach that was tried first and rejected

The first attempt derived the name by matching `Class::Method(` in the file body.
It scored `real_name` 972 → 2,611 and CONVERTED → 1,087, and it is wrong four ways.
An adversarial audit found all four with concrete cases:

- **Misattribution.** `search()` takes the first match anywhere in the file.
  `src/game/stages/Stage/_ZN5Stage13UpdateMessageEv.cpp` reports `Message::UpdateWindow` — a
  one-line stub for a *dependency* class declared above the real function.
  `src/game/actors/Coffin/_ZN6Coffin13InitResourcesEv.cpp` matches a bare `MeshCollider::LoadFile`
  forward declaration that ends in `;` and is defined in another file entirely.
- **Override of a correct failure.** Four files whose own symbol is a genuinely
  unidentified `func_ov*` placeholder (`func_ov006_020c8f20`, `func_ov006_020cb030`,
  `func_ov064_02117220`, `func_ov079_02126f8c`) had their verdict flipped to pass
  by an unrelated declaration elsewhere in the file.
- **Dropped qualifiers.** `Sound::Player::SetPlayableSeqCount` came out as
  `Player::SetPlayableSeqCount`, colliding with the unrelated real `Player` class.
- **Cannot match a ctor or dtor at all**, since neither has a return type before
  the `::`. That excluded **749** already-converted destructor files — exactly the
  case that motivated the change.

Demangling has none of these failure modes: the symbol is unambiguous, it is the
file's own, and `demangle.py` already handles nested names, ctors, dtors and
thunks. Every one of the cases above resolves correctly under it.

## Two holes left open on purpose

The audit's last finding is that `real_name` had been an **accidental backstop**.
While every mangled file failed it, it did not matter that the other criteria are
evadable. Fixing `real_name` exposes that. Measured against the 1,558:

| hole | files affected |
|---|---:|
| calls a `func_<addr>` / `FUN_<addr>` placeholder, which `no_mangled_refs` does not test | 347 |
| a fake local stand-in `struct` instead of including the real header | 121 |
| `(int)this + 0x154` — offset arithmetic laundered through an int cast, dodging `RAW_OFFSET` | 4 |
| tripped by at least one | **453** |

Closing all three would put CONVERTED at **1,105 (9.8%)**; closing the first and
third alone at **1,209 (10.7%)**.

They are not closed here, and the reason is scope rather than doubt. Closing the
`func_<addr>` hole takes `no_mangled_refs` from 6,521 to 1,431 tree-wide, making it
the tier's binding constraint instead of `real_name`, and it **de-credits 204 of the
426 currently banked files** — nearly all plain C files like `LoadFile.c` and
`GiveCoins.c` with no bearing on the C++-method defect this change fixes. Bundling
a 204-file backslide into a correction about method names would make one number
answer two questions, and would bury the finding above.

Each is marked `KNOWN HOLE` at its regex in `tools/tiers.py`. They want one PR each,
with the backslide discussed on its own terms.

## Consequences for the worklist

Under the corrected metric the cohort structure changes, and so does the answer to
"what is the next big slice":

```sh
4/5 cohort  3,565 files       (was 3,898)
real_name still the sole blocker for most of them
```

Of the files still failing `real_name`, the ones whose name is *recoverable* are
those a named class's vtable claims. `tools/rtti_vtables.py` reports 2,663
overriding slots across 426 classes; 626 of them hold a placeholder, 622 of those
have a source file, and for **433** an ancestor's slot supplies the method name
directly (`InitResources`, `Behavior`, `Render`, `CleanupResources`,
`OnYoshiTryEat`, ...). That is the naming worklist, and it is evidence-driven
rather than guessed.

See also `[[validator-what-it-proves]]`, `[[langmode-metric-is-textual]]` and
`[[reformat-silently-disables-gates]]` — this is the fourth measured instance of a
gate in this tree rewarding the wrong thing, and the second where documenting or
doing the work correctly made the number worse.
