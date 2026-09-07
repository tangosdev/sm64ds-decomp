# Static-initializer ownership report

`tools/sinit_owners.py` produces a review-only map from each compiler-generated
`__sinit_*` function to its likely original translation unit. It never moves a
source or edits linker configuration.

Generate the current TU evidence and report:

```powershell
python tools/rtti_extract.py --check
python tools/rtti_vtables.py
python tools/tu_map.py
python tools/sinit_owners.py
```

The report is written to the ignored files `build/sinit-owners.json` and
`build/sinit-owners.tsv`. Limit inspection to one overlay with:

```powershell
python tools/sinit_owners.py --module ov045 --show all --no-write
```

## Evidence model

The linker separates an input object's `.text`, `.init`, `.ctor`, data and BSS
contributions. Consequently, the nearest text address is not evidence of an
initializer's owner. The report follows this graph instead:

```text
__sinit -> touched global -> non-initializer consumers -> candidate TU
```

Only directional globals—assignment destinations, constructed objects and
destructor-registered objects—can produce high confidence. Read-only constants
referenced while copying aggregate data are recorded, but cannot establish an
owner. If the source does not expose a directional target, the report falls back
to all referenced globals and caps that inference at `medium`.

`ownership_globals` lists every directional target. `mapped_ownership_globals`
lists the subset with a recovered source consumer that maps to a candidate TU,
while `unconsumed_ownership_globals` lists targets with no recovered source
consumer. A `high` row means the observed consumer edges agree on one TU; it does
not claim that every directional target already has a recovered consumer.
`consumer_coverage` is `full`, `partial`, or `none` by that direct-source measure.
Use full-coverage rows as the first bulk-audit queue. For partial rows, invert ROM
relocations through intermediate data tables before treating the unconsumed
targets as owned; the source scan intentionally does not guess those indirections.

Constructor/destructor class names, an optional asset-layout report, and module
ordering provide independent evidence. The classifications mean:

- `proven`: the initializer is licensed by a TU manifest whose status is
  `link-verified`, `ready`, or `promoted`;
- `high`: a curated manifest inventories the initializer, or its touched globals
  resolve to one non-conflicting, class-labelled TU in a module whose map is not
  under-segmented;
- `medium`: one constructor-type, ordered, or asset hypothesis exists without
  strong consumer ownership evidence; unique consumers in anonymous or
  under-segmented clusters are deliberately capped here;
- `ambiguous`: independent signals disagree or consumers span multiple TUs;
- `count-only`: the initializer is only a module-level count/order witness.

An equal number of candidate TUs, `__sinit` functions and `.ctor` entries creates
an ordered hypothesis. It does not create proof. TUs without static initialization
can sit between initializer-bearing TUs, and the TU map itself documents that
sinits are a count/check rather than a text-boundary detector.

The four-byte empty initializer stubs usually remain `count-only` unless a curated
manifest or ordered anchors place them. Do not assign them from body evidence that
does not exist.

## Promotion gate

The report is a research queue. Before moving an initializer into a reconstructed
TU, declare the inferred globals in a shadow source and let mwccarm generate its
own `.init` and `.ctor` output. Compare the generated initializer bytes,
relocations, registration order, data/BSS layout, linked module and complete ROM.
Only that compiler-generated and linked result can promote ownership from a report
candidate into production structure.
