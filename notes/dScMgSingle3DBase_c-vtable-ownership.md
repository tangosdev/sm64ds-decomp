# Owning `_ZTV19dScMgSingle3DBase_c` — the two prerequisites

The promoted TU `src/minigames/d_s_mg_single3_d_base.cpp` emits this class's
`_ZTV`/`_ZTI`/`_ZTS` today, and they byte-compare against the cartridge. They are
nonetheless carried as `deadstrip-data` licences rather than owned outright
(`production_mode: "intact-object"`, the mode #2054 landed and #2057 first used).
Two things block ownership. Both are measured, not suspected.

## 1. The emitted table is an 18-slot prefix of a 36-slot cartridge table

`dScMgBase_c` leaves vtable slots 18-35 undeclared, so mwcc emits only as far as it
has been told. `romdata_check` scores the result PARTIAL: a verified byte-exact
prefix, not a disagreement. Ownership needs the full 36. See
`notes/dScMgBase_c-slots-18-35.md` for the slot-by-slot map and its two hazards.

## 2. [data_ov006_0213e42c](../config/arm9/overlays/ov006/symbols.txt) is two objects glued into one delink entry

The entry is configured as 0x1c bytes at [ov006](../config/arm9/overlays/ov006/symbols.txt):0x0213e42c. Dumped, it is:

| range | bytes | what it actually is |
|---|---|---|
| 0x0213e42c..0x0213e440 | `c2 c4 bf c0 c1` (5 words) | a real five-entry file-id table |
| 0x0213e440..0x0213e448 | `00000000`, `0213bc64` | `_ZTV19dScMgSingle3DBase_c`'s ABI preamble |

The first twenty bytes have a genuine reader in this very TU — the slot-33 override
does `func_ov004_020adc68(data_ov006_0213e42c[GetGameLanguage()])`. The last eight are
the offset-to-top word (0) and the typeinfo pointer (0x0213bc64, which is exactly
`_ZTI19dScMgSingle3DBase_c`), sitting immediately below the address point at
0x0213e448. `OI.VTABLE_PREAMBLE = 8`.

This is the case PR #2061 documents as **anti-pattern 3's exception**: a delinker
entry overlapping the storage start of a vtable the TU is about to supply is not
merely droppable, it is *unkeepable* — once the TU emits those eight bytes, no symbol
of that name exists in the linked binary and `dsd check symbols --fail` rejects it.

The twist here, which #2061's [ov047](../config/arm9/overlays/ov047/symbols.txt) case does not have: this entry is **not purely delinker-invented**. Twenty of its twenty-eight bytes are a live data object. So the
fix is not "drop it" but **split it** — retain [data_ov006_0213e42c](../config/arm9/overlays/ov006/symbols.txt) at 0x14 bytes (0x0213e42c..0x0213e440) so its reader keeps resolving, and let the TU supply the
eight preamble bytes. Dropping the whole entry would strand the language table.

Apply #2061's discriminator either way: restore the entry at its full 0x1c extent on
top of the ownership change and rebuild. A **new** symbol error beyond the baseline
set proves the shortening was required.

## Not a defect in what is landed today

Nothing above affects the current promotion. The TU reads indices 0-4 only, all
inside the genuine table, and the `deadstrip-data` licence explicitly discards this
object's copy of the vtable in favour of the cartridge's. The overrun in the entry's
configured extent is pre-existing and inert until someone tries to own the storage.
