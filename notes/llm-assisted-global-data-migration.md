# LLM-Assisted Global Data Migration

This note explains the global-data migration problem in `sm64ds-decomp` and proposes a practical way to use static analysis, LLM inference, and runtime ROM instrumentation to reconstruct likely global ownership.

**Status, 2026-10-17:** the counts and initial tooling restrictions below describe
the [2026-08-14 design snapshot](global-data-migration.md), not current migration
coverage. Source-built data and promoted TUs have since changed that starting
point. [The evidence collector](../tools/data_evidence.py) implements the first
static evidence packet described here. It does not establish semantic identity,
historical ownership, or a successful compiled-data comparison. An opt-in
`rombuild.py` check now verifies requested linked providers and bytes or BSS
storage layout. Pointer-target semantics and runtime tracing remain separate work.

It builds on the repository design note:

- [Global data migration — what it would take to source-build `.data` / `.rodata` / `.bss`](global-data-migration.md)

## 1. The problem in plain language

Think of the ROM as a LEGO model the project is rebuilding from instructions.

The decompilation rewrites the game's **actions**—its functions—in C and C++.
Those functions also need stored values: variables, constants, strings, and lookup
tables. Until each such object is migrated, its storage is supplied from the
original ROM rather than generated from a source definition.

For example, reconstructed code may contain:

```cpp
extern int playerLives;
```

This does not create `playerLives`. It promises the linker that some other object
will provide it. For an unmigrated global, that provider is generally a generated
gap object containing content delinked from the original ROM.

Conceptually:

```text
Our compiled source              Original-ROM gap objects
-------------------              ------------------------
Functions                        Global storage
References to globals  ------->  Data and BSS symbols
```

Consequently, reconstructing a function does not itself reconstruct the data that
function uses.

The original 2026-08-14 inventory counted:

- 11,394 function symbols
- 15,409 initialized or read-only data symbols
- 4,973 BSS symbols

These are historical symbol counts, not a current count of unmigrated objects.

## 2. What migrating a global means

Eventually, an `extern` declaration should be replaced by a real definition:

```cpp
int playerLives;
```

It cannot simply be changed in place. The gap object already supplies storage at the original address. Defining it in source without changing the delink layout can create duplicate or overlapping content.

A proper migration must:

1. Identify the symbol's exact original address range.
2. Carve that range out of the ROM-derived gap object.
3. Compile the replacement definition from source.
4. Place the compiled section into precisely the carved range.
5. Verify that its address and every affected byte remain correct.

This is conceptually the same process already used to migrate functions.

## 3. The good news: `dsd` already supports it

The underlying delinking format already accepts `.data`, `.rodata`, and `.bss` ranges. A single file entry can also declare several sections, such as a function and the global storage associated with its translation unit:

```text
Compiled object
├── .text    function code
├── .rodata  constants
├── .data    initialized writable data
└── .bss     zero-initialized storage
```

`dsd` can carve those ranges out of gap objects, arrange the resulting sections in address order, and select the compiled object when the entry is marked `complete`.

No new `delinks.txt` schema is required for ordinary `.data`, `.rodata`, or `.bss`.
The original design identified enrollment, eligibility, isolation, verification,
and reporting as the integration work needed around that support.

## 4. Historical tooling restrictions and the enduring contract

At the original design snapshot, the function-oriented tooling imposed these
restrictions; inspect the current implementation before treating one as a blocker:

- `enroll.py` only produced function candidates and emitted one section range per entry.
- `eligible.py` expected exactly one `.text` content section and one global function symbol.
- `objisolate.py` preserved the selected function while discarding other sections.
- `check_data_definitions.py` rejected definitions that would conflict with gap-object symbols.
- Reporting attributed reconstructed coverage to functions rather than global data.
- Data and BSS symbols lacked explicit sizes.

These restrictions protected the layout from silent shifts. The enduring
requirement is to generalize the license from:

> A source object contains exactly one declared function.

to:

> A source object contains exactly the functions and data ranges licensed by its delink entry, with the expected symbol sizes and sections, and nothing else.

## 5. The data sections

| Section | Meaning | Example | Relative difficulty |
| --- | --- | --- | --- |
| `.bss` | Zero-initialized storage | `int count;` | Easiest |
| `.data` | Writable initialized data | `int count = 5;` | Hard |
| `.rodata` | Constants, strings, and tables | `const int table[] = {1, 2};` | Hard |
| `.ctor` | Static-constructor machinery | Initialization function pointers | Special case |

### 5.1 Why BSS is the logical first step

For a definition such as:

```cpp
int counters[4];
```

the ROM does not need to store sixteen literal zero bytes. It reserves sixteen bytes of RAM and clears them during startup. A source definition therefore primarily needs to reproduce the correct address, size, and alignment.

Initialized `.data` is harder because it may contain both literal values and relocations. A pointer initializer must reproduce the appropriate relocation against the correct symbol at the correct offset. `.rodata` similarly requires exact constants, strings, padding, ordering, and alignment.

### 5.2 The BSS size problem

An address-only data or BSS entry in `symbols.txt` does not establish object size.
The next distinct symbol address or section end gives a candidate boundary:

```text
boundary span = next boundary address - current symbol address
```

That span is an upper bound under the assumption of separate, non-overlapping
objects; aliases and interior labels require review. It is not necessarily the
variable's historical C++ size. For example:

```text
symbol A starts at 0x1000
symbol B starts at 0x1004
```

The original source might have defined a two-byte `short` followed by two bytes of padding. Defining only a `short` could make the compiler emit a two-byte contribution and shift every following BSS symbol.

Matching the ROM may initially require an explicitly padded representation, with the uncertainty documented rather than hidden.

Keep the boundary span, any explicitly supplied inspection range, and the
evidence for the actual object size separate. Access widths and offsets, array
indexing, constructor behavior, and compiler-emitted symbol sizes can strengthen
the size claim. Supplying `--size` to the collector selects bytes to inspect; it
does not prove that the selected extent was one original object.

## 6. The ownership problem

The current decompilation commonly stores one function per source file. The original game used normal translation units containing multiple functions and globals:

```cpp
// Possible original shape: d_a_moray.cpp

UnagiResources resources;
int instanceCount;

void daMoray_c::Init() { /* ... */ }
void daMoray_c::Update() { /* ... */ }
void daMoray_c::Render() { /* ... */ }
```

The reconstructed tree may instead contain separate files for each function. The linker normally does not preserve enough information to state with certainty which original `.cpp` file owned a particular global.

Placing a definition in the only current source file that references it is therefore only a guess. It may produce an identical ROM without recovering the historical source structure.

The project should distinguish three questions:

1. **Mechanical range:** Which section, address, and size must the symbol occupy?
2. **Semantic identity:** What does the symbol represent, and what is its likely type?
3. **Historical ownership:** Which original translation unit probably defined it?

The first can often be proven. The second and third may remain probabilistic.
Track a proposed descriptive name separately from the type and owner. A useful
name such as `identityQuaternion` need not be Nintendo's original identifier.
Likewise, a current delinks owner is a build-configuration fact, not proof of the
historical source file or of the object actually selected by a completed link.

## 7. Why LLM-assisted inference is promising

This is a suitable use of an LLM because ownership depends on combining many weak, heterogeneous clues rather than applying one exact rule.

The LLM should act as an evidence-ranking detective. Deterministic tooling must remain responsible for addresses, ranges, alignment, relocations, linking, and bit-exact verification.

### 7.1 Useful static evidence

Potential evidence includes:

- The symbol's module or overlay
- Every code and data relocation referencing the symbol
- Functions already assigned to reconstructed TU clusters
- Constructor and destructor calls receiving the symbol's address
- `__sinit` construction groups
- Function, vtable, RTTI, and registration relationships
- Adjacent data symbols and repeated layout patterns
- Access widths and offsets suggesting a scalar, array, or structure
- Resource identifiers, actor IDs, stage IDs, and strings used by referencing code
- Link order and compiler-generated artifacts associated with a TU

An evidence-ranking system might treat signals approximately as follows:

| Strength | Evidence |
| --- | --- |
| Hard constraint | The symbol and proposed owner are compatible with the same module or overlay |
| Very strong | Its constructor is called by a `__sinit` routine associated with the reconstructed TU |
| Strong | Independent binary references cluster in one well-supported TU; shared users have been considered |
| Strong | Constructor or destructor calls reveal the likely class type |
| Supporting | Observed runtime accesses cluster in one subsystem across contrasting scenarios |
| Medium | Nearby globals form a coherent layout and behavior cluster |
| Medium | Initialization and destruction timing matches the candidate subsystem |
| Weak | The nearest reconstructed function references it |
| Very weak | The symbol is merely physically close to a function in the linked image |

No single weak signal should decide ownership.

Configured relocations describe known pointer relationships, including their
target offsets; they are not a complete record of CPU reads and writes. Source
text matches may be declarations, comments, or aliases. Label both evidence kinds
explicitly and do not count repeated mentions as independent behavioral evidence.
Use module-qualified identities throughout: different overlays can occupy the
same RAM address, and a symbol name or numeric address alone can be ambiguous.

## 8. `__sinit` is a clue, not the destination file

The global-data design note correctly rejects thousands of symbols as easy pilot migrations when their only current reference comes from a synthetic one-function `__sinit_*` file. Moving definitions directly into those files would not reconstruct a meaningful source layout.

However, `__sinit` routines may be among the richest inference sources. Static initialization frequently exposes relationships equivalent to:

```cpp
SomeType::SomeType(&data_ov016_02114d4c);
```

This can reveal:

- The global's likely class type
- Which objects were constructed together
- Their construction order
- A relationship between the initializer and a larger original translation unit

The initializer should therefore be treated as a fingerprint:

```text
__sinit routine
    -> constructed global cluster
    -> related function cluster
    -> candidate original translation unit
```

The globals should not simply be left in the artificial `__sinit` source file.
Constructor calls also distinguish an object's initial storage from its later
contents. An eight-byte resource handle may begin as zeroed BSS and receive its
file ID during initialization; the handle is bookkeeping for a model or animation
file, not the asset file itself. Preserve construction order, destructor
registration, and relocation targets when rebuilding this behavior.

## 9. Runtime instrumentation

Static analysis can be supplemented with targeted emulator tracing. Because the symbol addresses are already known, an instrumented emulator can log reads and writes for selected ranges.

A useful event might contain:

```text
frame=1248
cpu=ARM9
overlay=ov016
address=0x02114d4c
operation=write32
old_value=0x00000000
new_value=0x00000001
pc=daMoray_c::InitResources+0x38
callers=dScStage_c::Load -> Actor::Spawn -> daMoray_c::InitResources
```

Important fields include:

- ARM9 or ARM7
- Active overlay or module
- Address and symbol identity
- Read or write operation
- Access width
- Old and new values
- Executing PC mapped to a recovered function
- Reconstructed call context when available
- Frame, stage, room, actor, or other scenario context
- Whether the access came from normal CPU execution, an interrupt, or DMA

The active overlay must be recorded because overlays may reuse memory addresses.

### 9.1 What runtime evidence can reveal

Across controlled play sessions, traces can reveal:

- Which functions actually read and write the global
- Whether it becomes read-only after initialization
- Its lifetime and reset behavior
- Whether it changes per frame or only during loading
- Likely type and size from access widths and value patterns
- Whether adjacent globals act like one structure or array
- Which actors, stages, resources, or subsystems activate it
- Whether a supposedly subsystem-specific global is actually shared

Runtime evidence does not directly reveal the source file that declared the variable. It provides semantic and community-of-use evidence that can significantly improve an ownership ranking.

### 9.2 Prefer targeted experiments

Tracing every memory access for an entire playthrough would create excessive noise. A better workflow is:

1. Select a small set of target BSS addresses.
2. Instrument reads and writes only for those ranges.
3. Run deliberately contrasting scenarios.
4. Summarize the resulting traces deterministically.
5. Ask the LLM which hypothesis best explains the observations.

For a daMoray_c-related candidate, contrasting experiments might include:

```text
Run A: Enter the stage without spawning daMoray_c
Run B: Approach and activate daMoray_c
Run C: Spawn and despawn daMoray_c repeatedly
Run D: Trigger another water enemy without daMoray_c present
Run E: Leave and reload the stage
```

The LLM can also recommend the next discriminating experiment. If two likely owners are `d_a_moray.cpp` and a shared water-enemy resource TU, running another water enemy without daMoray_c may separate the hypotheses.

## 10. Per-object evidence reports

For a whole-tree backlog, first run [tools/data_inventory.py](../tools/data_inventory.py):

```sh
python tools/data_inventory.py --json build/data-inventory.json --summary-json build/data-inventory-summary.json
```

It groups configured symbols by module and address, preserving aliases and unknown
object sizes. Its section coverage also counts unnamed storage, so a symbol census
cannot hide unclaimed prefixes or padding. Initialized storage and BSS are reported
separately. Code labels, code-embedded data, and addresses outside configured sections
remain separate categories. All ownership is declared configuration; no compiler or
link proof is imported. The report contains no ROM bytes.

Research groups combine a declared complete owner's entries with unowned entries
having one identified source in the same module among resolved configured references.
Cross-module consumers remain dependency evidence, not owner candidates. Other references
may still be ambiguous or lack a source owner; the groups retain those counts.
That second relationship is only a research lead, not a historical ownership decision. Shared users, conflicts,
and unassigned addresses remain visible. In particular, a generated initializer
file is a clue to a TU, not an acceptable destination for all data it constructs.
Ambiguous overlay sets remain unresolved instead of becoming concrete pointer
targets. Source mentions include comments and declarations and never count as
definitions. Freeze the checkout while collecting; input hashes are rechecked but
are not an atomic filesystem snapshot.

Start a review with [tools/data_evidence.py](../tools/data_evidence.py):

```sh
python tools/data_evidence.py --symbol data_02075b04 --module arm9
python tools/data_evidence.py --symbol data_02075b04 --module arm9 --size 0x10 --json build/quaternion-evidence.json
python tools/data_evidence.py --symbol data_02075b04 --no-rom
```

Repeat `--symbol` for several objects. Use `--module` to disambiguate a name;
`--no-rom` produces metadata-only evidence when extracted ROM inputs are absent.
The default output is human-readable; `--json` also writes a structured report.
An explicit `--size` applies to a single `--symbol` only.

The collector reports configured symbols and section boundaries, incoming and
outgoing configured relocation references with modules and offsets, source text
references, declared delinks owners, and manifest references. When ROM bytes are
available, it adds hashes of the inspected ranges and small word previews. A
boundary-derived range is labeled as such, not presented as a recovered type size.
The collector does not infer a type, rename a symbol, select a historical owner,
trace accesses, or compare a compiled object against the ROM. Compiled proof
remains `not-run`, even if a manifest records an earlier successful check.

Keep the generated observations with a reviewed record for each object:

| Record | Required distinction |
| --- | --- |
| Identity | Module/overlay, address, configured symbol, section |
| Extent | Candidate boundary, inspected range, proposed object size/alignment, and the evidence for each |
| Contents | Original bytes/range hash and pointer targets plus addends; BSS has no stored initializer bytes |
| References | Configured relocations, source mentions, decoded binary accesses, and runtime observations are separate categories |
| Placement | Configured owner, actual linked provider, and proposed historical owner are separate claims |
| Interpretation | Type, descriptive name, and historical owner each have evidence, alternatives, contradictions, and uncertainty |
| Reproducibility | Exact source commit, dirty-tree status or patch fingerprint, tool/compiler versions, ROM version and input fingerprints |
| Verification | Each check's status, command, exact tested commit, and retained output/artifact |

Use `confirmed by metadata`, `strong inference`, `tentative`, or `unknown` for
each interpretation separately. Avoid unexplained decimal confidence scores.
Use `not-run`, `pass`, or `fail` for verification; an inference label cannot stand
in for a byte check. The full reviewed record is the target workflow, not a claim
that the initial collector populates every field.

### First researched group: ov047 cart resources

Independent static/binary research on the PR #4015 source snapshot
`62ebf5bbc03f406d09f384a784e57c0c397c24f8` established the following proposals.
These readable names are reconstructions, not recovered Nintendo variable names.
They have not yet replaced the configured symbols.

| Configured symbol | Proposed descriptive name | Format and evidence |
| --- | --- | --- |
| `data_ov047_02112408` | `gKm3KurumaResources` | 12 bytes: model handle pointer, collision handle pointer, CLPS block pointer; raw pointer words, configured relocations, and the ov002 loader agree |
| `data_ov047_02112638` | `gKm3KurumaModelFile` | 8-byte model handle; initializer supplies runtime asset handle 0x067f, resolving to NitroFS file 0x04d0 and the cart BMD |
| `data_ov047_02112630` | `gKm3KurumaCollisionFile` | 8-byte collision handle; runtime handle 0x0680 resolves to NitroFS file 0x04d1 and the cart KCL |
| `data_ov047_02111ab4` | `gKm3KurumaClps` | 8-byte CLPS header plus one 8-byte entry; semantic identity strong, historical TU owner unproven |

The handle constructor at 0x02017e0c stores a resolved `u16` file ID at +0,
a `u8` reference count at +2, and a 32-bit loaded-file pointer at +4; +3 is
padding. These are runtime bookkeeping objects, not the BMD/KCL files themselves.
The fieldless public SharedFilePtr declaration and local wrappers still need a
consumer-wide layout audit before a shared-header change.

The CLPS header contains magic `CLPS`, entry stride 8, and a likely entry count 1.
The count interpretation is supported by the asset's 12 collision prisms all
referencing entry 0; the game's lookup directly checks stride but does not read the
count field. The entry at header+8 has two words, 0x00000fc0 and 0x000000ff.
Its separate configured label is an interior address, not evidence for an
independent 24-byte object. Sixteen following 0xff bytes remain padding/ownership
research. The existing production map selects this block from a retail gap.
Neither the descriptor's consumer relationship nor a coincident cross-overlay
actor-table address establishes the block's historical owner.

The complete catalog also identifies the existing compiler-generated RTTI,
type-name, vtable, constructor-table entry, and two destructor-registration nodes,
plus the already named spawn profile. The vtable's 136-byte storage starts at
0x02112444; its public address point is 0x0211244c. The registration nodes are
three pointers (next node, destructor, object), established by runtime routine
0x020731dc. Preserve these ABI symbols and compiler generation instead of replacing
them with arbitrary variable names.

The ten declared addresses tile 252 bytes across `.ctor`, `.data`, and `.bss`
without internal holes; this is catalog arithmetic, not a new compilation proof.
The additional CLPS block and its interior entry bring the research catalog to
12 records. Cross-module base-class RTTI is an external dependency and belongs
with the base-class review, not with the cart's ownership expansion.

The private research packet is `build/kuruma-research/research.json` in the resumed
PR #4015 worktree; it retains disassembly, asset resolution, map references,
confidence, and unresolved questions. Actual migration must update the source,
symbol/configuration/manifest references, initializer machinery, and complete
section claims coherently, then rerun full-TU and final-provider verification.
No game-source changes are made by the inventory tooling.

### What establishes source-built accuracy

A matching final image can still contain the original gap bytes. Verification
must establish that the new source definition actually supplied the linked range.
The opt-in provenance checker follows the selected compiled object and section
through the module-qualified linker map, final ELF symbols, linker selectors,
and actual object paths. It rejects a gap-object fallback, including a ROM-derived
object with the same basename as the expected compiler object.

Create an ignored request file, for example `build/data-requests.json`:

```json
[
  {"module": "ov047", "symbol": "data_ov047_02112408", "source": "src/game/actors/d_a_obj_km3_kuruma.cpp", "size": 12},
  {"module": "ov047", "symbol": "data_ov047_02112638", "source": "src/game/actors/d_a_obj_km3_kuruma.cpp", "size": 8}
]
```

Then run:

```sh
python tools/rombuild.py -j16 --no-rom --data-provenance-requests build/data-requests.json --report-json build/data-build-report.json
```

Each size is an explicit review claim. The tool checks it against configured
bounds and the exact emitted and linked symbol extent; it does not infer a C++
type or establish a missing historical size. Requests currently support `.data`,
`.rodata`, and `.bss` with one complete `src/` owner. A failed requested proof
fails the build; ordinary builds are unchanged. Existing full-build gates still
run, and their failure cannot be replaced by a successful per-object check.

The report's `dataProvenance` field contains separate results:

- `source-bytes-verified`: the requested compiler object supplied the exact
  initialized range and every linked byte equals the retail reference.
- `source-storage-layout-verified`: the requested compiler object supplied the
  BSS symbol at the requested address and size, with NOBITS storage, input-section
  alignment, and configured section bounds checked. Startup clearing and
  constructor behavior remain `not-run`.
- `refused`: provider, placement, extent, byte comparison, or freshness evidence
  failed. Missing outputs and changed inputs cannot produce a pass.

Source, headers, configuration, Python tools, and compiler/linker executables are
fingerprinted before compilation. The generated build configuration, actual
post-isolation objects, LCF, and object list are also bound. Expected output files
are removed immediately before the linker runs; the receipt hashes the new map,
ELF, and module images. Retain the report as evidence for those exact inputs;
an old receipt is not automatically valid for a new checkout. The static
collector does not import receipts or turn their claims into a current pass.

Use the documented `dsd` 0.11.0 pin. `rombuild.py` and `tubuild.py linkcheck`
check it before preparing build inputs and record its path, version, and SHA-256
under `dsdTool`. A later tool version can change the linker script even when
the C++ source and compiler are unchanged; such a build is a separate toolchain
experiment, not evidence for the pinned pipeline.

For initialized data, compare the complete affected linked bytes after relocation:
values, pointers, ordering, alignment, padding, and section tails. Check the
module-qualified relocation destination and addend as well as the numeric word;
overlay address reuse can hide a wrong destination. Report compared extents and
lengths explicitly so an unchecked trailing range cannot disappear behind a pass.

For BSS, matching ROM file bytes is insufficient because the reserved zeroed
storage is not stored as initialized bytes. Check its linked address, size,
alignment, section extent, neighboring boundaries, and zero-initialization
contract. Constructor code, constructor pointers, and destructor registrations
need their own byte and relocation proof where they initialize these objects.

Bind every proof to the exact candidate commit and ROM/module fingerprints. A
report from an older PR revision is historical evidence, not a pass for the
current head. ROM-range hashes describe the input; by themselves they prove
nothing about newly compiled output or original source ownership.

## 11. Implementation phases

### Phase 1: Collect static evidence (initial collector)

Use the collector above for a small batch and review the reported boundaries,
configured relationships, text references, and ROM ranges. Preserve missing or
ambiguous evidence explicitly. Current configuration and manifest claims must
remain distinguishable from independently measured binary facts.

### Phase 2: Establish linked provenance and complete comparisons (in progress)

1. Implemented for explicit requests: actual compiled-object/link-map provenance,
   exact symbol address/size checks, complete initialized-byte comparisons, and
   BSS storage layout. ROM gap fallback cannot satisfy a source-provider request.
2. Still required for broader migration acceptance: module-qualified pointer
   destination/addend checks and BSS startup/constructor checks. Numeric pointer
   bytes alone cannot distinguish destinations at the same overlay address.
3. Regression controls cover changed pointer bytes and tails, mis-sized BSS,
   wrong overlay/provider, stale inputs, and an unchanged final image supplied by
   a gap object. A same-address wrong-overlay pointer needs its own semantic
   relocation check before that stronger claim can be made.
4. Verify both individual licensed ranges and affected module extents, recording
   any exclusions and the exact tested source/input fingerprints.

### Phase 3: Enrich identity and ownership evidence (remaining)

Decode binary accesses and array strides; connect constructor/destructor groups,
resource IDs and paths, RTTI/vtables, and existing TU candidates. Review a small
sample manually before scaling. Propose separate types, names, and owners with
contradictions and the next useful experiment. Existing reconstructed names and
comments are leads, not independent confirmation of themselves.

### Phase 4: Add targeted runtime experiments (remaining)

Implement the address watches and controlled scenarios in section 9. Record the
active overlay, scenario, PCs, access widths, and relevant values; include
interrupt/DMA coverage or its absence. Use traces to discriminate hypotheses,
not to assert that unobserved accesses never happen.

### Phase 5: Migrate reviewed objects and retain uncertainty

Move definitions in coherent TU groups using the current production build path.
Run the applicable byte, relocation, declaration, and reference checks at the
candidate head; retain the per-object evidence beside the PR's proof. Keep a
mechanically verified object marked historically uncertain when ownership is
still an inference. Tooling and source migrations belong in separate PRs.

## 12. Limitations

Runtime and LLM inference cannot restore information that no longer exists with mathematical certainty.

- An unobserved access does not prove that an access never occurs.
- Rare paths may not execute during testing.
- Interrupt and DMA activity may not have normal call context.
- Optimized ARM code can make stack reconstruction imperfect.
- Tightly coupled TUs may be observationally indistinguishable.
- Moving a definition between two files in the same module may produce an identical ROM.
- A bit-exact build can prove layout correctness without proving historical source ownership.

The system should therefore preserve uncertainty and evidence rather than forcing every symbol into a supposedly definitive owner.

The collector is the starting point for that review. Deterministic build checks
establish source provenance and layout accuracy; human and LLM review can assess
meaning, rank ownership hypotheses, and choose the next discriminating experiment.
