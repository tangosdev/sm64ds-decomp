# The cartridge disagrees with 259 of our class names, and that gates promotion

Measured on `origin/main` `01c4d350a`, reproducible with `python tools/rtti_name_audit.py`.

```
total _ZTV symbols in config    541
  RTTI name AGREES with ours    276
  RTTI name DISAGREES           259
  typeinfo word unreadable        6
```

Just under half the vtable-bearing classes in the tree carry a name the ROM contradicts.
That has always been true; what is new is that it is now measured exhaustively, and that
it turns out to control something other than readability.

## A coined name blocks TU promotion

Promoting a translation unit consolidates the class's RTTI, so the compiler emits
`_ZTI<OurName>` and `_ZTS<OurName>`. The cartridge holds those records under the class's
real name, at a different length, so no ROM symbol can license them. `tubuild` refuses:

```
HOMELESS     _ZTS18daObjKm2_Ami_Bou_c  STB_LOPROC .data size=0xa
HOMELESS     _ZTI18daObjKm2_Ami_Bou_c  STB_LOPROC .data size=0xc
COLLIDES-GAP _ZTV18daObjKm2_Ami_Bou_c  STB_GLOBAL .data size=0x84 already at ov045:0x02112dbc
```

`daObjKm2_Ami_Bou_c` is `daObjKm2_Ami_Bou_c` in the ROM. This was found the expensive way, on
[#2066](https://github.com/tangosdev/sm64ds-decomp/pull/2066): the destructor work was
correct and landed, and the promotion it was meant to unlock could never have succeeded.
No amount of TU-boundary evidence would have changed that.

The mechanism holds across everything that has already landed, not just the one failure.
Of the 23 manifests on `main` with `status: promoted`:

| | |
|---|---|
| own a vtable, name **agrees** with the ROM | **15** |
| own a vtable, name **disagrees** | **0** |
| own no vtable, so RTTI never arises | 8 |

Nothing name-mismatched has ever promoted. Check the name before doing the work.

## The evidence chain

Direct, and self-checking. From a `_ZTV` address `V`, the word at `V-4` is the typeinfo
pointer; a `__si_class_type_info` record is `[vptr][name ptr][base ptr]`, so word 1 leads
to the `_ZTS` string. That string is length-prefixed, so a correct read verifies itself:

```
_ZTV18daObjKm2_Ami_Bou_c @ 0x02112dbc            (ov045, base 0x021111a0)
  [V-8] offset-to-top = 0x00000000
  [V-4] typeinfo      = 0x02112d74
  TI[0] vptr          = 0x0209a764
  TI[1] name ptr      = 0x02112d80 -> '18daObjKm2_Ami_Bou_c'
  TI[2] base's _ZTI   = 0x021089ec
```

`18` is exactly `len("daObjKm2_Ami_Bou_c")`. `TI[2]` gives the ROM-proven direct base for
free, which is worth having when a hierarchy is in doubt.

## Three ways to measure this wrong

All three produced confident wrong numbers before they were caught.

**Read `config/` from `origin/main`, not from a checkout.** A checkout that is behind will
audit names that have already been renamed. Measuring the same tree from a stale working
copy returned 265/543 instead of 259/541 -- six renames had landed and two `_ZTV` symbols
were gone entirely.

**Resolve each address in its own overlay first.** Many overlays share a base address --
ov045, ov046 and ov047 are all at `0x021111a0` -- so a reader that picks an image by
address, or that falls back to `arm9` first, silently reads a different overlay. The tool
takes the overlay from the path of the `symbols.txt` the symbol came from.

**Use `extracted/arm9_dec.bin`.** `extracted/dsd/arm9/arm9.bin` is compressed and its
strings are not at their link addresses.

And one reporting rule: **an unreadable typeinfo word is unmeasured, not an agreement.**
Six are unreadable -- `daDemo_c::simpleModel_c`, `daDemo_c::anmModel_c`, `daOts_c`,
`daObjMaruta_c`, `daDsnBase_c`, `daObjFallBlock_c` -- and folding them into the agreeing
column would overstate what is promotable by six classes.

## What this does to the inline-destructor wave

The 41-TU inline-destructor census was ordered as a destructor workstream. Intersected
against this audit, it is mostly a renaming one:

| | |
|---|---|
| census classes whose name **agrees** | **11** -- all already have manifests on `main` |
| census classes whose name **disagrees** | **48** |
| census classes owning no `_ZTV` | 19 |

The destructor flip itself is unaffected and still lands independently -- it changes
`virtual ~X();` to an inline body and rewrites the two per-symbol files as forcing
helpers, none of which touches RTTI. But the *promotion* half of that list is spent on
the agreeing side. Work it top-to-bottom expecting 41 promotions and 48 of them will fail
at `tubuild`, each after a full TU reconstruction.

The disagreements concentrate: ov002 has 35, `arm9` 23, ov064 12, ov022 9, ov029 8. An
ov002 rename pass would unblock more promotions than any other single piece of work.

## Using it

```
python tools/rtti_name_audit.py                  # summary, first 25 disagreements
python tools/rtti_name_audit.py --all            # every disagreement
python tools/rtti_name_audit.py --class daObjKm2_Ami_Bou_c # one class, with the chain
python tools/rtti_name_audit.py --json out.json  # every row
```

`--class` is the one to run before starting a promotion. It costs a second and it is the
difference between a promotion that can land and one that cannot.

Related: [tu-promotion-conventions.md](tu-promotion-conventions.md).
