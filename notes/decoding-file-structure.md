# Decoding file structure: the source ↔ ROM map

Per-TU header essays used to paste ROM addresses, vtable locations, and
shard tables into every source file. That information now lives in
machine-checked files, and this note is the map to it. The rule: prose
here describes the *system*; concrete addresses live in `config/` and
are verified by the build, so they are never copied into docs or
headers.

## Source file → ROM range

[delinks.txt](../config/arm9/overlays/ov002/delinks.txt) enrolls each
source file as one `complete` span (one object emits one contiguous
`.text`):
```sh
    grep -A2 "src/game/actors/d_a_tree.cpp" config/arm9/overlays/ov002/delinks.txt
```
## ROM address → source file

There is no lookup tool; scan the owning overlay's `delinks.txt` for
the span containing the address:
```sh
    python3 -c "
    import re
    addr = 0x020ec100
    cur = None
    for ln in open('config/arm9/overlays/ov002/delinks.txt'):
        m = re.match(r'^(\S+):$', ln)
        if m:
            cur = m.group(1)
            continue
        m = re.search(r'start:(0x[0-9a-f]+)\s+end:(0x[0-9a-f]+)', ln)
        if m and cur and int(m.group(1), 16) <= addr < int(m.group(2), 16):
            print(cur, ln.strip())
    "
```
## Function → ordinal and legacy shard

The old `[N] address file` header tables are the TU manifest rows:
`ordinal` + `address` + `legacy_source` per function, e.g.
[ov002/daTree_c.json](../config/tu_manifest.d/ov002/daTree_c.json).
Where a TU has no manifest yet, the ordinal is the function's position
in ROM-address order (see below). The resync gate keeps
compiler-numbered symbols in these rows honest.

## Function address and size

`symbols.txt` carries one row per function with its size, so the old
per-function `name, address, size` banners are a grep away:
```sh
    grep "_ZN8daTree_c6RenderEv" config/arm9/overlays/ov002/symbols.txt
```
## Key function and vague linkage

Which TU emits a class's `_ZTV`/`_ZTI`/`_ZTS` group, and why, is
recorded in that TU's manifest notes (key-function selection,
vague-linkage reason per row, deadstrip proofs) — e.g. the notes in
[ov002/daTree_c.json](../config/tu_manifest.d/ov002/daTree_c.json).
Where the answer constrains future edits, a one-liner stays in the
header next to the key function; the essay lives in the manifest.

## Which names to trust

Real RTTI-proven names versus reconstructions:
[symbol-name-provenance.md](symbol-name-provenance.md) for the system,
the TU manifest's notes for the per-class verdict (including coined
predecessors), and
[rom-name-glossary.json](../config/rom-name-glossary.json) for what
the Japanese names mean and how confident each reading is.

## Logbook banners

`/* recovered: ... */` banners are deliberately preserved nowhere:
they narrated the recovery pass, and every fact in them is visible in
the code they sat on.

## VTables, RTTI, type strings

[symbols.txt](../config/arm9/overlays/ov002/symbols.txt) carries one
row per `_ZTV*` / `_ZTI*` / `_ZTS*` with its address.
[actor-vtables.md](actor-vtables.md) explains how to read them
(address point, slot 0, base inference):
```sh
    grep "_ZTV8daTree_c" config/arm9/overlays/ov002/symbols.txt
```

## Slot numbers

Shared headers annotate every virtual with `/* slot N */`
([dActor_c.h](../include/dActor_c.h) is complete through slot 30); a
derived class's new virtuals append after its base's last slot. The
vtable bytes themselves are verified by the build, which is what pins
the annotations.

## Symbol → defining file

Sources carry `// @symbol` markers above each function, backed by the
same TU manifests:
```sh
    grep -rn "@symbol _ZN8daTree_c6RenderEv" src/
```
## Class size

The TU's `classInit` factory carries the measured allocation size as
a literal (`return new` forms) — that literal, not a comment, is the
size evidence.

## History: merged shards, coined names

`git log --follow` on the TU file. What used to be "assembled from"
tables and "the decomp used to call it X" notes is commit history.

## Relocation expectations

`relocs.txt` next to each overlay's `symbols.txt` lists the expected
relocations (`from:`/`kind:`/`to:`/`module:`) — the file behind every
old "a relocation the ROM build checks" note:
```sh
    grep "0x020ec004" config/arm9/overlays/ov002/relocs.txt
```
## Field status in structures

`unk_` is an unidentified field, `pad_` an explicit observed gap;
renaming either cannot change codegen, only offsets matter. The
full epistemics stamp survives where `gen_header.py` wrote it, e.g.
[Clipper.h](../include/Clipper.h).
