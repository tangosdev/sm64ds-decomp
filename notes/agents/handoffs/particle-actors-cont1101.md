# Handoff: particle-actors-cont1101

This document describes this commit. The queue records its immutable output SHA.

## Identity and status

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/4570; producer session `codex-particle-cont1101` (Codex).
- Branch `cleanup/particle-shadow1101`; base and workflow `ad5a9c03fffe174eaec000ce99ab55a5325c3b9f`; input `b5921f4e1c6f480e4c9ff106470173b03daeaab6`.
- Producer candidate. Independent source acceptance and integration remain pending. Further particle shadows remain. The camera file also overlaps open #4106 and needs composition review.
- Earlier camera, trampoline and star changes and limitations are described in their preceding handoffs. Local logs and object files are under ignored `build/`.

## Actor batch

Replace local `Particle::System::NewSimple` declarations with `Particle__System.h` in `d_a_bombking.cpp`, `d_a_gmch.cpp`, `d_a_kuri_king.cpp`, and `d_a_obj_hatena_block.cpp`. Their return values are discarded; the shared declaration returns `void *`. All call arguments and call sites are unchanged. Re-key each TU manifest's existing compiler-generated `from` symbols; preserve all destination mappings.

Compiled base and candidate objects were compared in order: all allocated sections matched in name, type, size, alignment, flags and bytes; all relocation records matched after pairing the compiler-renumbered local symbols by section, order, offset and size. The checks cover BSS as writable `SHT_NOBITS` and data as writable `SHT_PROGBITS` rather than treating equal zero bytes as proof of section identity.

| TU | Functions matched | Allocated sections | Relocations | Paired local symbols |
|---|---:|---:|---:|---:|
| `daBombking_c` | 52/52 | 152 | 634 | 51 |
| `daGmch_c` | 37/37 | 83 | 313 | 24 |
| `daKuriKing_c` | 55/55 | 124 | 592 | 29 |
| `daObjHatenaBlock_c` | 41/41 | 124 | 442 | 50 |

Each `tubuild.py verify --no-write --verdict` reports clean function bytes and relocation destinations, but exits 1 on one unlicensed initializer section. Each unchanged input has the same one-section warning; none of these is claimed as a clean promotion verdict. See `build/particle-d_a_*-{base,head,fixed}.log`, corresponding `{base,head}.o` and `build/particle-actor-object-comparison.py`.

`python tools/rombuild.py -j16 --no-rom`: exit 0 for all seven replacements together on the specified base, 106/106 modules exact, 11,395 source functions match, ROM data zero differing. `build/particle-actors-rombuild.log`.

The earlier camera+trampoline `prepush_linkcheck` had 231 verified, one initializer `BLIND-17`, and no blocking errors; `build/particle-object-comparison.py` independently compares every trampoline allocated section and relocation with the clean base object. The exact final commit still needs its range-scoped linkcheck, declaration agreement, current-base revalidation, and independent source review. No source paths, shared header definitions, asset payloads or original-name claims were changed.