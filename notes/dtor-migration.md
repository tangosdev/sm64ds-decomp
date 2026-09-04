# Migrating destructors to real C++

> **Status: historical research note, partially superseded.** The backlog counts are
> frozen, and the absolute D0 conclusion in section 0 was disproved after relocation
> checking and object isolation were fixed. `tools/d0_migrate.py` records and automates
> the current method; `python tools/langmode_audit.py` and
> [`cpp-tu-current-state.md`](cpp-tu-current-state.md) provide live status. The failure
> analysis below remains useful unless a section is explicitly marked superseded.
>
> **2026-08-24 sweep:** `d0_migrate.py` landed 73 more D0s in one pass; the nine it
> could not keep are each blocked by something named below -- five by the §3
> inline-base-dtor convention (SignPost, daDsnBase_c, daObjMaruta_c, daObjFallBlock_c,
> daOts_c -- their headers define `~Class() {}` so derived destructors inline it, and
> the tool's out-of-line definition is a redefinition), one by member arrays
> (dScEntry_c, two `__destroy_arr` calls), one by a member whose type's destructor is
> still a `func_` shell (dCapEnemy_c, daDossyCap_c, daObjMarioCap_c -- the CapIcon
> ordering documented in include/daDossyCap_c.h), and one by a conditional body no
> empty destructor reproduces (dWipe_c's needsCleanup test). daObjAbuku_c's D1 landed
> the same day through `dtor_probe.py`.
>
> **The link caught what verify could not, twice more.** dFdDummy_c's D0 and
> daKrb_c's D0+D1 all returned `(True, '2004/b56')` from `build_pin.verify` and then
> mismatched the real relink by 1-3 words -- the §5 lesson repeated: a byte check
> wildcards relocated words, so a `bl` resolving to the wrong variant compares equal
> in isolation. daKrb_c derives from dCapEnemy_c, whose own D0 stays hand-written for
> the operator-delete reason its D2 file records; the same wall reaches the child.
> All three files were reverted to their hand-written `.c` after `rombuild -j16`
> reported 104/106, and the build returned to 106/106 exact. **`build_pin.verify`
> passing is not sufficient evidence a destructor migration lands; only the full
> relink is.**

> **2026-08-27 correction, from the enemy-family sweep** (`notes/cpp-conversion-enemies.md`):
> two of the nine D0s the 2026-08-24 sweep could not keep now migrate cleanly.
> `dCapEnemy_c` and `daObjMarioCap_c` were listed as blocked by "a member whose type's
> destructor is still a `func_` shell (the CapIcon ordering)"; `d0_migrate.py` keeps
> both today and `rombuild -j16` reports 106/106 exact. Re-test a blocked class before
> trusting the list above -- these entries were fixed underneath the note, not by it.
>
> The daKrb_c wall in the paragraph above is **not class-specific**. `daTrs_c`'s D0+D1
> hit the identical rejection (`cand _ZN11dCapEnemy_cD2Ev (0x0200651c) != config`
> *0x020aedbc*:[ov002](../config/arm9/overlays/ov002/symbols.txt)), so the cross-module
> `dCapEnemy_c::~dCapEnemy_c` duplicate blocks the destructor pair of **every** `dCapEnemy_c` descendant, not just daKrb_c.
>
> Operational note: `d0_migrate.py` performs the `.c` -> `.cpp` rename but does NOT
> re-point `delinks.txt`. It reports `OK` anyway; the next `rombuild` fails in
> `layout_check`. Fix the enrolment path by hand after every run.

**Scope:** what actually blocks a `D1` file from becoming `Class::~Class()`, measured
rather than assumed. Companion to `runbook-type-reconstruction.md` §7, which recorded
the wall before objisolate came down and is no longer the whole story.

The remaining backlog is mostly destructors: at the time of writing
`tools/langmode_audit.py --by-class` reports **D0 258** unmigrated (3 migrated) and
**D1 192** (74 migrated) against **374** ordinary methods. So this is the bulk of the
work, and most of it is blocked by four specific things rather than by one wall.

## 0. D0 is not migratable, ever [SUPERSEDED]

This conclusion is retained as research history, not current guidance. A real
`X::~X()` can emit D2/D0/D1 together, and `objisolate` can keep the D0 contribution;
`tools/d0_migrate.py` is the current executable path.

`D0` is the *deleting* destructor. The compiler generates it; nobody writes it. Renaming
its file `.cpp` changes an extension without migrating anything and raises
`cpp_still_handspelled` -- the issue #821 debt the langmode ratchet correctly rejects.
`src/_ZN5EnemyD0Ev.c` explains this in place. **Count D0 out of the backlog**; the
reachable target is D1.

## 1. The tooling lies about size mismatches, and it lies with a number

`build_pin.verify` reports `999 word(s) differ` for **any** size difference. It is
`match.compare`'s sentinel (`return False, 999` when `len(target) != len(cand)`), not a
count. A candidate one instruction short and a candidate that is unrelated to the target
produce the identical message.

That matters here because destructor candidates are usually *near* misses, and the
sentinel hides exactly the signal you need. Use `tools/dtor_diff.py`, which prints both
sizes and the two disassemblies side by side:

```sh
python tools/dtor_diff.py src/_ZN5ModelD1Ev.cpp _ZN5ModelD1Ev 0x02016d20 0x38 arm9
# pin 2004/b56   ROM 0x38   candidate 0x30   delta -8 bytes
```

`tools/dtor_probe.py` writes `Class::~Class() {}` over every candidate whose header
already has a real base and a virtual destructor, verifies it, and puts the original
back if it does not match. Nothing is kept that did not reproduce the ROM.

## 2. A delete-expression emits no null test at this pin

`Model::~Model` frees one owned buffer. `delete (char *)transformsBuf` compiles and is
**8 bytes short**: mwcc calls `operator delete` unconditionally where the ROM tests the
pointer first. Writing the test out is what matches:

```cpp
if (transformsBuf)
    ::operator delete(transformsBuf);
```

Do not assume a delete-expression carries its guard. Measure it.

## 3. The base destructor must be INLINE for a derived one to match

This is the big one, and it is unproven only in the sense that nothing has landed on it
yet -- the measurement is unambiguous.

`Scene::~Scene` in the ROM stores **two** vptrs and then calls `ActorBase::~ActorBase`:

```arm
str r2, [r4]        ; _ZTV5Scene
str r1, [r4]        ; _ZTV12ActorDerived   <- ActorDerived's D2, INLINED
bl  ActorBase::~ActorBase
```

With `virtual ~ActorDerived();` merely *declared*, the compiler cannot inline it and
emits `bl _ZN12ActorDerivedD2Ev` instead -- one store where the ROM has two. Change the
declaration in `include/dBase_c.h` to a definition:

```cpp
virtual ~ActorDerived() {}
```

and `Scene::~Scene() {}` reproduces `0x0202e140` exactly, `(True, '2004/b56')`. So the
original sources defined these destructors **in the class body**, and every derived
destructor inlined them.

**The cost, and it is why this is not done yet.** An inline definition makes the base's
own D1 file a redefinition. A TU that merely includes the header emits nothing
(`_ZN12ActorDerivedD1Ev is not in the object`), so the base's D1 file has to force the
out-of-line copy -- an explicit destructor call in a throwaway function does it, and
verifies:

```cpp
#include "dBase_c.h"
void _force(void *p) { ((ActorDerived *)p)->~ActorDerived(); }   // (True, '2004/b56')
```

objisolate drops the forcing function's `.text`, so the object is still clean. But this
is a tree-wide convention change affecting every base class in the hierarchy, and it
wants its own PR and a human's opinion on the idiom. **Roughly 60 unmigrated D1 files
have the two-vtable-store shape**, so that is the size of the prize.

**2026-08-25: the idiom landed, and its D0 twin was found.** Nine files over five
classes (SignPost, daDsnBase_c, daObjMaruta_c, daObjFallBlock_c, daOts_c) replaced
their hand-spelt D1/D0 `.c` shells without touching the inline definitions the
derived classes rely on:

- **D1** is the forcing call above, one per file, named `_force_<Class>D1`.
- **D0** has no source expression that names it, but the vtable needs it as a
  symbol, and the vtable is emitted by the TU defining the class's key
  function. Defining that key function in the D0 file (body irrelevant --
  objisolate discards the duplicate `.text` and the `.data`) makes mwcc emit
  the destructor variants alongside the vtable, and the file binds to D0.
  SignPost's `Kill`, daObjMaruta_c's `OnHitByMegaChar`, daObjFallBlock_c's
  `Kill` and daOts_c's `CleanupResources` all verified plain and strict.
- **A class with no key function has no D0 path.** daDsnBase_c declares no
  non-inline virtual, so nothing can force its vtable out of a TU; explicit
  instantiation (`template class daDsnBase_c;`) is rejected by b56 as
  illegal. Its D0 stays a `.c` shell -- the only one of the ten attempted.

## 4. objisolate cannot yet isolate a multiple-inheritance destructor

`ModelAnim : Model, Animation` and its derivatives fail eligibility with:

```sh
isolate: _ZTV9ModelAnim: unexpected reloc type=2 addend=44
```

`tools/objisolate.py` corrects the primary vptr store, whose addend is
`VTABLE_PREAMBLE` = 8 (offset-to-top + typeinfo), because the ROM's `_ZTV<C>` symbol IS
the slot array while mwcc's addresses the object's start. A secondary base's vptr points
further in -- 44 for `ModelAnim`, i.e. 8 + 7 slots + the secondary preamble -- and
objisolate refuses rather than guessing.

**The refusal is correct as written.** The comment above `VTABLE_PREAMBLE` records what
a wrong correction cost last time: a clean link that wrote every isolated class's vptr 8
bytes high across 76 functions and 34 modules, caught by nothing but the byte compare.
The generalisation (`_ZTV<C> + (addend - 8)`) is the same arithmetic the primary already
uses and is verifiable the same way, but it belongs in its own PR with its own evidence.

Until then `ModelAnim`, `ModelAnim2` and `BlendModelAnim` byte-match as real destructors
and **cannot be enrolled** -- migrating them makes them ineligible, which is a
regression, so they are deliberately left as C files.

## 5. `config/rombuild-exclude.txt` is right about Model, and now there is a number

The exclude list says its C++ destructor entries "linked cleanly but produced the wrong
bytes ... where a deleting/complete variant resolves differently in a real relink than it
does when linkcheck verifies the match in isolation". That was tested again here, because
a real `Model::~Model()` is a different object from the hand-written C file and might
plausibly have resolved differently.

It does not. With `_ZN5ModelD1Ev` removed from the exclude list:

```sh
eligible          10805 -> 10806, gained _ZN5ModelD1Ev, lost nothing
rombuild -j16     105/106 exact, 1 mismatching
                  arm9  _ZN5ModelD1Ev  0x02016d20 size 0x38   1 word
```

`build_pin.verify` returns `(True, '2004/b56')` for that same file, which is the whole
lesson of `notes/`' relocated-words rule: a byte check wildcards relocated words, so a
`bl` to the wrong target compares equal. **The entry stays.** Anyone re-testing this
should expect the same one word and not read `build_pin`'s True as a contradiction.

## 6. What does work today, and it is the tier everything else chains into

With the eleven engine base destructors and `MeshCollider` migrated, the classes an
actor destructor calls into are real C++. The shape that reproduces from an empty body
is: a real base in the header, a virtual destructor, members whose types are real (or
have no destructor), and no user logic in the ROM body. Where the ROM body *does* have
logic it is usually one statement -- a handle released (`MeshCollider`), a buffer freed
(`Model`), a list unlinked (`ShadowModel`).
