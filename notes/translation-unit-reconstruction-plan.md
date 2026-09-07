# Translation-unit reconstruction and consolidation plan

> **Status: active design history with shadow tooling implemented.** `tools/tubuild.py`,
> `src_tu/`, and `config/tu_manifest.d/` now exercise candidate translation units,
> but manifest entries still do not control the normal ROM build and production
> promotion remains dry-run only. The current `config/**/delinks.txt` enrollment is
> authoritative until a candidate passes the gates below and is promoted. Use
> [`cpp-tu-current-state.md`](cpp-tu-current-state.md) for generated current counts and
> compatibility blockers; keep this document as the architecture and safety rationale.

**Related notes:**

- [`tu-boundaries.md`](tu-boundaries.md) — current translation-unit boundary recovery
- [`global-data-migration.md`](global-data-migration.md) — requirements for source-building `.data`, `.rodata`, and `.bss`
- [`llm-assisted-global-data-migration.md`](llm-assisted-global-data-migration.md) — evidence-ranked global ownership and runtime tracing
- [`rom-build.md`](rom-build.md) — hybrid ROM build and verification
- [`objisolate.md`](objisolate.md) — current compiled-object isolation
- [`plan-cpp-language-mode.md`](plan-cpp-language-mode.md) — C-to-C++ migration and its safety model

---

## 1. The problem

The project currently uses roughly one source file per reconstructed function. That shape is excellent for independent matching:

- one candidate compiles in isolation;
- one address range is enrolled at a time;
- unmatched neighbors remain supplied by ROM-derived gap objects;
- a failed candidate can be removed without affecting unrelated functions;
- concurrent contributors can claim and review small units of work.

It is not the shape of the original program and should not be mistaken for the final recovered source architecture.

The ROM contains thousands of functions, but the original build compiled ordinary C and C++ translation units. One original `.cpp` could contain:

- several methods of one class;
- methods of multiple related classes;
- free functions and local helpers;
- vtables and RTTI contributions;
- constants and lookup tables;
- initialized globals and BSS storage;
- static initialization and destruction machinery.

The current tree therefore conflates two different units:

| Unit | Purpose | Desired granularity |
| --- | --- | --- |
| Matching unit | Reproduce one machine-code range safely | One function or one licensed section |
| Source unit | Represent the original program intelligibly | One recovered translation unit |

The final program will still contain approximately eleven thousand functions. The goal is not to remove functions. The goal is to stop requiring approximately eleven thousand physical `.c` and `.cpp` files.

## 2. Evidence that consolidation is plausible

At the time `notes/tu-boundaries.md` was measured:

- `src/` held 11,122 one-function files;
- 11,088 functions in code-bearing modules were assigned to 501 candidate TUs;
- 388 candidate TUs carried at least one class label;
- 270 boundaries were high-confidence, 107 medium-confidence, and 50 low-confidence;
- 30 of 74 code-bearing modules were independently corroborated by static-initializer counts.

The 501 value is a lower bound rather than a final source-file count. `main`, `ov006`, `ov007`, and several other modules are known to be under-segmented. Even allowing for that, the likely destination is hundreds or perhaps high hundreds of meaningful `.cpp` files—not one file per function.

Classes and TUs are not interchangeable:

- Two interleaved class spans can prove that the classes shared a TU. MontyMole and MontyMoleRock are the canonical example.
- A large class can have methods defined across several TUs.
- A TU can contain unnamed helpers that carry no class label.
- Vtables generally anchor the TU containing the relevant key function or destructor, not every TU containing methods of that class.

The recovered structure must follow evidence about object boundaries rather than enforce a one-class-one-file aesthetic.

## 3. Why this should be a new workstream

Prior file-consolidation discussion correctly rejected merging files merely to reduce build time. Translation-unit reconstruction has a different purpose:

1. Give the recovered C++ a truthful and usable organization.
2. Give vtables, RTTI, globals, and static initialization a meaningful owner.
3. Allow whole-object and whole-TU verification in addition to function verification.
4. Make a future host build consume coherent classes and subsystems rather than thousands of compiler-oriented fragments.
5. Provide the structural evidence required by global-data migration.

This is not a formatting cleanup. It generalizes the build's trusted unit from "one source object containing exactly one function" to "one source object containing exactly the functions and data ranges licensed by a recovered TU manifest."

## 4. Design principles

### 4.1 Preserve the working pipeline

The current `src/` tree and hybrid ROM build remain authoritative until a consolidated TU reproduces its complete licensed contribution. A design experiment must never silently reduce source-built coverage.

### 4.2 Use one canonical source after promotion

Temporary duplication is acceptable during a short-lived shadow experiment. Permanent parallel implementations are not. Once a TU is promoted, its one-function predecessors are deleted and the consolidated `.cpp` becomes canonical.

### 4.3 Reuse the exact toolchain

The TU pipeline must use the existing:

- mwccarm `2004/b56` compiler and flags;
- symbol and relocation configuration;
- extracted user-provided ROM;
- dsd delinking and LCF generation;
- module and ROM comparison gates.

It must not create an independent build whose success means something different from `rombuild.py` success.

### 4.4 Separate inference from verification

TU boundaries and ownership can be inferred with confidence levels. Addresses, sizes, relocations, compiler output, and final linked bytes are deterministic facts. The manifest must preserve that distinction.

### 4.5 Fail on unlicensed output

A compiled object is eligible only when every defined function, object, content section, relocation, and range is expected by its manifest. Extra output is a failure, not something the linker may silently ignore.

---

## 5. Proposed transitional layout

Add a small staging tree rather than copying all of `src/`:

```sh
sm64ds-decomp/
├── src/                         # Current production source and promoted TUs
├── src_tu/                      # Temporary shadow TUs; not enrolled by default
│   ├── actors/
│   ├── engine/
│   └── overlays/
├── include/                     # Shared headers; no duplicate include tree
├── config/
│   └── tu_manifest.d/           # Recovered TU membership and licensed ranges;
│       ├── _meta.json           #   one file per entry, under its module, so
│       └── ov002/               #   two TU PRs never touch the same file
│           └── daCamTag_c.json
├── tools/
│   ├── tu_manifest.py           # The manifest's only on-disk-shape reader/writer
│   └── tubuild.py               # TU create/compile/verify/link/promote entry point
└── build/
    └── tu/                      # Generated objects and reports; gitignored
```

Rules for `src_tu/`:

- It is an incubator, not a second permanent source tree.
- Current enrollment tools must ignore it unless explicitly invoked by `tubuild.py`.
- Only selected pilot TUs are added; there is no mass copy of 11,000 files.
- It reuses the real headers from `include/`.
- A shadow TU never contributes to the normal ROM build accidentally.
- Promotion moves the file into its intended permanent `src/...` location and deletes the superseded one-function files.

The name `src_tu/` is intentionally transitional. If the approach succeeds at scale, consolidated files should ultimately live under meaningful directories inside `src/`.

## 6. TU manifest

`build/tu_map.json` is analysis output. A source migration needs a reviewable manifest that records both evidence and build obligations.

An illustrative entry:

```json
{
  "id": "ov062/Chuckya",
  "module": "ov062",
  "source": "src_tu/actors/Chuckya.cpp",
  "promoted_source": "src/actors/Chuckya.cpp",
  "status": "shadow",
  "boundary_confidence": "high",
  "boundary_evidence": [
    "overlapping class interval",
    "contiguous linker run",
    "sinit count corroboration"
  ],
  "sections": [
    {
      "name": ".text",
      "start": "0x021xxxxx",
      "end": "0x021xxxxx"
    }
  ],
  "functions": [
    {
      "symbol": "_ZN7Chuckya8BehaviorEv",
      "address": "0x021xxxxx",
      "size": "0x000xxxxx",
      "legacy_source": "src/_ZN7Chuckya8BehaviorEv.cpp",
      "ordinal": 0
    },
    {
      "symbol": "_ZN7Chuckya6RenderEv",
      "address": "0x021xxxxx",
      "size": "0x000xxxxx",
      "legacy_source": "src/_ZN7Chuckya6RenderEv.cpp",
      "ordinal": 1
    }
  ],
  "data": [],
  "bss": [],
  "notes": []
}
```

The manifest should eventually record:

- module or overlay;
- source path and intended promoted path;
- ordered function membership;
- expected symbol names, addresses, and sizes;
- licensed `.text`, `.init`, `.rodata`, `.data`, `.bss`, and `.ctor` ranges;
- aliases, thunks, weak symbols, and destructor variants;
- boundary confidence and its evidence;
- data-ownership confidence separately from mechanical placement;
- legacy source paths to remove on promotion;
- current lifecycle state.

Suggested lifecycle states:

| State | Meaning |
| --- | --- |
| `mapped` | Candidate boundary exists only in analysis output |
| `shadow` | Consolidated source exists but contributes nothing to the normal build |
| `text-verified` | All declared function output matches in TU compilation context |
| `link-verified` | Scratch link reproduces the complete licensed text range |
| `data-verified` | Declared data/BSS contributions also pass their gates |
| `promoted` | Consolidated source is canonical and legacy files are removed |

## 7. Proposed `tubuild.py` interface

One entry point should orchestrate the existing lower-level tools rather than create several competing workflows.

```sh
python tools/tubuild.py list
python tools/tubuild.py inspect ov062/Chuckya
python tools/tubuild.py create ov062/Chuckya
python tools/tubuild.py compile ov062/Chuckya
python tools/tubuild.py verify ov062/Chuckya
python tools/tubuild.py linkcheck ov062/Chuckya
python tools/tubuild.py promote ov062/Chuckya
```

### 7.1 `list`

Show candidate TUs with:

- module;
- address interval;
- function count;
- matched/source-built function count;
- class labels;
- boundary confidence;
- sinit corroboration;
- disqualifying complications such as data sections, destructors, pragmas, or unmatched functions.

This becomes the pilot-selection worklist.

### 7.2 `inspect`

Print the ordered functions, existing paths, static evidence, section ranges, data references, special compiler pragmas, and any conflicts that prevent consolidation.

### 7.3 `create`

Create a shadow manifest entry and an initial consolidated `.cpp` without changing enrollment.

The generator may extract function bodies and assemble a starting file, but it cannot blindly concatenate the complete legacy files. One-function sources commonly repeat or contradict:

- local shadow class declarations;
- typedefs;
- `extern` declarations;
- include sets;
- macros;
- compiler pragmas;
- helper declarations.

The generated file is a starting point requiring deliberate normalization around the real project headers.

### 7.4 `compile`

Compile the shadow TU once with the pinned matching toolchain. Store the untouched compiler object and an inspection report under `build/tu/<id>/`.

The report should inventory:

- every defined symbol;
- every ELF content section;
- which repeated `.text` section contains each function;
- section sizes, alignment, and order;
- relocations by section;
- unexpected `.data`, `.rodata`, `.bss`, `.init`, or `.ctor` output;
- compiler diagnostics and selected language mode.

### 7.5 `verify`

Verify the multi-function object against the manifest and ROM:

1. The defined function set equals the manifest function set.
2. Functions occur in the expected order.
3. Each function has the expected size.
4. Function bytes match the original range.
5. Relocation slots and destinations match.
6. Every non-text content section is explicitly licensed.
7. There are no undeclared global or weak object definitions.
8. There are no duplicate or overlapping section claims.

Reuse the existing `match.py`, `fdiff.py`, relocation, and ELF-parsing logic rather than implementing a different definition of "match."

Example report:

```python
TU ov062/Chuckya

MATCH  Chuckya::~Chuckya          0x021xxxxx  size 0x0xx
MATCH  Chuckya::InitResources     0x021xxxxx  size 0x0xx
MATCH  Chuckya::Behavior          0x021xxxxx  size 0x0xx
DIFF   Chuckya::Render            0x021xxxxx  7 instructions differ
EXTRA  .rodata                    size 0x14, not licensed

Result: shadow only; promotion refused
```

### 7.6 `linkcheck`

Create an isolated scratch configuration that replaces the original TU's contiguous range with the candidate object. It must not edit the real `config/` or `build/` in place.

Run, at minimum:

- dsd delink and LCF generation;
- source compilation with the pinned toolchain;
- scratch module link;
- linked range comparison;
- `dsd check symbols --fail`;
- module byte comparison;
- full ROM build when the module passes.

This stage verifies contribution ordering, padding, alignment, and relocation destinations that independent function comparisons cannot fully settle.

### 7.7 `promote`

Promotion is explicit and is refused unless every required gate is green.

It should produce a reviewable change that:

1. Moves the shadow source to its permanent `src/...` path.
2. Replaces the legacy per-function delink entries with the licensed TU entry.
3. Deletes exactly the superseded one-function source files.
4. Updates `srcpath.py` consumers and port manifests when necessary.
5. Marks the manifest entry `promoted`.
6. Runs the complete validation set.

`promote` must have a dry-run mode that prints every planned file deletion and configuration change.

---

## 8. Initial implementation: require a complete text TU

The safest first version should not attempt partial contribution from a multi-function object.

For the pilot:

- every function in the candidate TU must already source-build;
- the consolidated object must reproduce every function in TU context;
- the complete contiguous `.text` contribution is replaced as one unit;
- all undeclared data output remains forbidden;
- the existing per-function files remain active until the scratch link passes.

This avoids making partial object splitting a prerequisite for proving the basic architecture.

Transition for one pilot:

```text
N verified one-function files
        ↓ create
one shadow TU in src_tu/
        ↓ compile + verify
all N functions match in the merged compiler context
        ↓ scratch link
complete TU text range reproduces
        ↓ promote
one canonical src/.../*.cpp; N legacy files removed
```

If only 8 of 10 functions still match after consolidation, production remains unchanged. The two differences are investigated; the TU is not promoted merely because most of it works.

## 9. Later implementation: partial TU contribution

After a complete-text pilot is reliable, the build can support readable consolidated source before the entire TU is finished.

Potential model:

1. Compile the canonical multi-function `.cpp` once.
2. Map each `STT_FUNC` symbol to its emitted `.text` section and relocations.
3. Produce isolated derived objects containing only licensed, matching function sections.
4. Continue supplying unmatched functions and undeclared data from ROM gap objects.
5. Fail if isolation changes symbol binding, relocations, alignment, or section semantics.

Conceptually:

```text
Foo.cpp
  ↓ mwccarm
Foo.o
  ├── Function A .text  → match → isolated source object
  ├── Function B .text  → match → isolated source object
  ├── Function C .text  → diff  → ROM gap object
  ├── Function D .text  → match → isolated source object
  └── Foo data          → not licensed → ROM gap object
```

This is a generalization of `objisolate.py`, not a source-level preprocessor that pretends the functions were compiled independently.

Partial-TU mode has a crucial semantic distinction:

> The function was compiled in the candidate original TU context, even if its final linker contribution was isolated for incremental migration.

That makes it a better stepping stone toward the original object than continuing to compile every function as a separate TU forever.

## 10. Why merged compilation can change previously matching functions

Moving unchanged function text into a shared `.cpp` does not guarantee identical object code. Potential causes include:

- file-scoped compiler pragma state;
- declaration and definition order;
- C versus C++ language mode;
- internal versus external linkage;
- shared constants and literal placement;
- vtable, RTTI, and key-function emission;
- static initialization;
- destructor and constructor variants;
- inline and template instantiation;
- local shadow declarations replaced by real headers;
- compiler-generated helpers and aliases.

The byte gate decides whether a consolidation is valid. A previously matched isolated function that differs in merged context is not automatically a regression in understanding—it is a new matching problem at the historically meaningful granularity—but source-built coverage must not fall until the replacement is ready.

Files using local `#pragma opt_*` settings deserve special scrutiny. Per-function files can accidentally turn TU-scoped compiler state into function-scoped state. Consolidation may expose that the matching workaround never described a plausible original TU.

## 11. Relationship to data migration

Translation-unit reconstruction should precede large-scale global-data placement.

Without it, moving a global definition into the only one-function file that references the symbol confuses "current user" with "original owner." The linker may still reproduce the ROM, but the source structure remains invented.

A promoted TU creates the correct container for:

- class vtables and RTTI;
- static `SpawnInfo` and resource descriptors;
- module-local lookup tables;
- zero-initialized resource state;
- initialized pointers and registrations;
- `__sinit` construction groups;
- constructor and destructor machinery.

The order of difficulty remains:

1. complete `.text` only;
2. `.text` plus one unambiguous BSS symbol;
3. multiple BSS definitions;
4. simple relocation-free `.rodata`;
5. initialized `.data` and relocation matching;
6. vtable/static-initializer/destructor TUs;
7. complete original-object reconstruction.

The LLM-assisted ownership pipeline in `llm-assisted-global-data-migration.md` should populate candidate ownership and semantic evidence. `tubuild.py` must enforce only the mechanically licensed result.

## 12. Verification layers

No single green check is sufficient. Verification should be layered:

| Layer | Question answered |
| --- | --- |
| Manifest validation | Is the proposed TU structurally self-consistent? |
| Object inventory | Did the compiler emit only expected symbols and sections? |
| Per-function comparison | Does every function reproduce its original bytes and relocations? |
| Whole-range comparison | Do ordering, padding, and adjacency reproduce? |
| Symbol check | Did every symbol retain its expected linked address? |
| Module comparison | Does the affected ARM9/overlay image reproduce? |
| ROM build | Does the complete hybrid ROM reproduce and boot under the normal pipeline? |

Required negative controls:

- Deliberately swap two function ordinals and confirm failure.
- Add an unexpected helper function and confirm failure.
- Add an unlicensed global and confirm failure.
- Mis-size a BSS definition and confirm symbol or module failure.
- Omit one function and confirm the build cannot report the TU as promoted.
- Duplicate a range claim and confirm manifest validation fails.
- Change a relocation destination while preserving instruction bytes and confirm link verification fails.
- Confirm no shadow file under `src_tu/` is accidentally enrolled in the production build.

## 13. CI and repository invariants

Suggested invariants:

1. Every promoted TU manifest entry names an existing canonical source.
2. Every legacy path listed by a promoted entry is absent.
3. No symbol is defined by two active source owners.
4. No declared module ranges overlap unless recorded as an alias.
5. The manifest's function order matches address order except for explicitly documented compiler artifacts.
6. `src_tu/` files never enter the ordinary build without a manifest-selected TU invocation.
7. A promotion may not decrease verified source-built bytes.
8. An unexpected content section is always an error.
9. `dsd check symbols --fail` runs whenever a source object contributes BSS or data.
10. Generated `build/tu/` artifacts are never committed.

The current one-function eligibility gate remains in place for legacy sources. A separate TU-aware gate handles manifest-backed objects until it is mature enough to subsume the old rule.

## 14. Pilot selection

Do not begin with `main`, `ov006`, `ov007`, or another known under-segmented module.

The first candidate should have:

- a high-confidence contiguous TU boundary;
- independent corroboration where possible;
- approximately 5–15 functions;
- all functions already source-built and matched;
- no local `#pragma opt_*` workarounds;
- no constructor/destructor variants;
- no unexpected data, BSS, RTTI, or vtable emission;
- no by-value class-parameter exclusions;
- clear existing headers or a small header blast radius;
- no active overlapping work claim.

The first milestone is intentionally boring:

> Several individually verified source files become one meaningful `.cpp`, and the complete ROM remains byte-identical.

The second pilot should deliberately contain two interleaved related classes in one TU. The third should introduce one well-understood BSS definition. Only after those succeed should the workstream target vtable/destructor TUs.

## 15. Phased implementation

### Phase A — Manifest and reporting

- Define and validate `config/tu_manifest.d/`.
- Convert selected `tu_map.json` records into reviewable candidates.
- Implement `tubuild.py list` and `inspect`.
- Add overlap, membership, order, and legacy-path gates.
- No build changes.

### Phase B — Shadow TU compiler

- Implement `create` and `compile`.
- Produce complete ELF inventories.
- Confirm current enrollment ignores `src_tu/`.
- Add negative tests for extra symbols and sections.

### Phase C — Complete-text pilot

- Implement per-function and whole-range verification.
- Scratch-link one high-confidence complete TU.
- Promote it only after module and ROM checks pass.
- Measure build time, codegen changes, review complexity, and tooling friction.

### Phase D — Partial-TU isolation

- Generalize object isolation to select declared function sections from a multi-function object.
- Preserve relocations and symbol bindings exactly.
- Allow a canonical TU to contribute its verified subset while gaps supply the remainder.
- Prove failure on omitted, duplicate, or extra sections.

### Phase E — BSS and simple data

- Add licensed object definitions to the manifest.
- Wire `dsd check symbols --fail` into the relevant build.
- Migrate one-symbol BSS owners first.
- Add deliberately mis-sized negative controls.

### Phase F — Vtables and initialized data

- Recover complete ownership clusters.
- Match vtable/RTTI/static-initializer contributions.
- Reproduce `.data` and `.rodata` relocations.
- Promote complete original-object candidates.

### Phase G — Scale and retire the staging tree

- Consolidate high-confidence overlays in coherent batches.
- Improve under-segmented module inference using new evidence.
- Move stable files into meaningful `src/` directories.
- Remove `src_tu/` once no shadow migrations require it.

## 16. Definition of done for a promoted TU

A TU is complete only when:

- its boundary has recorded evidence and confidence;
- it has one canonical source path;
- every expected function is present in the correct order;
- every function matches bytes and relocations;
- all emitted content sections are licensed;
- owned data and BSS, if any, have exact size and placement;
- symbol addresses match `symbols.txt`;
- the affected module reproduces;
- the complete ROM build passes;
- superseded one-function files are removed;
- no source-built coverage regresses;
- relevant port references and source-path indexes are updated;
- uncertainty about historical naming, typing, or ownership is documented rather than silently promoted to fact.

## 17. What not to do

- Do not copy all one-function files into a second tree.
- Do not merge by class name alone.
- Do not place globals in whichever current file happens to reference them.
- Do not weaken `check_data_definitions.py` globally.
- Do not allow the linker to ignore unexpected sections.
- Do not treat a green ROM build as proof that the intended source object contributed.
- Do not promote a TU whose consolidation causes matched functions to fall back to ROM bytes.
- Do not begin with a heavily under-segmented module.
- Do not turn `src_tu/` into a permanent alternative implementation.

## 18. Recommended immediate next action

Implement only Phase A and the read-only half of Phase B:

1. Select one high-confidence, fully matched TU from `build/tu_map.json`.
2. Add one manifest record.
3. Create one manually curated shadow `.cpp` under `src_tu/`.
4. Compile it with the exact pinned compiler configuration.
5. Inventory every emitted section and compare each function.
6. Publish the report without changing the ROM build.

That experiment will answer the most important unknown cheaply:

> How many already-matched functions remain matched when compiled together in their candidate original TU context?

If the answer is favorable, implement scratch linking and promotion. If it is not, the failure report will identify whether the primary obstacle is boundary inference, pragma state, language mode, shared declarations, or compiler-generated data.

---

## Decision summary

The one-function files are a matching scaffold, not the intended final C++ architecture. Keep that scaffold operational while adding a small manifest-backed shadow pipeline for reconstructed translation units. Prove one complete text-only TU end to end before supporting partial extraction or global data. Promote a consolidated file only when it preserves all byte, relocation, symbol, module, and ROM guarantees, then delete its superseded fragments.

This provides a controlled path from approximately eleven thousand verified function fragments to hundreds of coherent C++ translation units without sacrificing the hybrid ROM pipeline that made the current decompilation possible.
