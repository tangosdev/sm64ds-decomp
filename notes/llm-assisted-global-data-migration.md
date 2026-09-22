# LLM-Assisted Global Data Migration

This note explains the global-data migration problem in `sm64ds-decomp` and proposes a practical way to use static analysis, LLM inference, and runtime ROM instrumentation to reconstruct likely global ownership.

It builds on the repository design note:

- [Global data migration — what it would take to source-build `.data` / `.rodata` / `.bss`](global-data-migration.md)

## 1. The problem in plain language

Think of the ROM as a LEGO model the project is rebuilding from instructions.

The decompilation has rewritten most of the game's **actions**—its functions—in C and C++. However, almost all global objects, variables, constants, strings, and lookup tables are still supplied as pieces copied from the original ROM.

For example, reconstructed code may contain:

```cpp
extern int playerLives;
```

This does not create `playerLives`. It promises the linker that some other object will provide it. Today, that other object is generally a generated gap object containing content delinked from the original ROM.

Conceptually:

```text
Our compiled source              Original-ROM gap objects
-------------------              ------------------------
Functions                        Global storage
References to globals  ------->  Data and BSS symbols
```

Consequently, the project can reconstruct most executable functions while still source-building approximately none of the global data.

The scale is significant:

- 11,394 function symbols
- 15,409 initialized or read-only data symbols
- 4,973 BSS symbols

There are more unmigrated global-data symbols than function symbols.

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

No new `delinks.txt` schema is required for ordinary `.data`, `.rodata`, or `.bss`. The repository's own enrollment, eligibility, isolation, verification, and reporting tools are the blockers.

## 4. Why the repository tooling currently rejects globals

The safety tooling presently assumes that a reconstructed object contains exactly one function and no global data:

- `enroll.py` only produces function candidates and emits one section range per entry.
- `eligible.py` expects exactly one `.text` content section and one global function symbol.
- `objisolate.py` preserves the selected function while discarding other sections.
- `check_data_definitions.py` correctly rejects definitions that would conflict with gap-object symbols.
- Reporting attributes reconstructed coverage to functions rather than global data.
- Data and BSS symbols do not currently carry explicit sizes.

These restrictions protect the layout from silent shifts. They should not be disabled. They should be generalized from:

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

Data and BSS entries in `symbols.txt` provide addresses but not sizes. The distance to the next symbol gives an implied stride:

```text
implied stride = next symbol address - current symbol address
```

That stride is not necessarily the variable's historical C++ size. For example:

```text
symbol A starts at 0x1000
symbol B starts at 0x1004
```

The original source might have defined a two-byte `short` followed by two bytes of padding. Defining only a `short` could make the compiler emit a two-byte contribution and shift every following BSS symbol.

Matching the ROM may initially require an explicitly padded representation, with the uncertainty documented rather than hidden.

## 6. The ownership problem

The current decompilation commonly stores one function per source file. The original game used normal translation units containing multiple functions and globals:

```cpp
// Possible original shape: Unagi.cpp

UnagiResources resources;
int instanceCount;

void Unagi::Init() { /* ... */ }
void Unagi::Update() { /* ... */ }
void Unagi::Render() { /* ... */ }
```

The reconstructed tree may instead contain separate files for each function. The linker normally does not preserve enough information to state with certainty which original `.cpp` file owned a particular global.

Placing a definition in the only current source file that references it is therefore only a guess. It may produce an identical ROM without recovering the historical source structure.

The project should distinguish three questions:

1. **Mechanical range:** Which section, address, and size must the symbol occupy?
2. **Semantic identity:** What does the symbol represent, and what is its likely type?
3. **Historical ownership:** Which original translation unit probably defined it?

The first can often be proven. The second and third may remain probabilistic.

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
| Very strong | Most independent static references come from functions clustered into one TU |
| Strong | Constructor or destructor calls reveal the likely class type |
| Strong | Runtime access is exclusive to one subsystem or actor family |
| Medium | Nearby globals form a coherent layout and behavior cluster |
| Medium | Initialization and destruction timing matches the candidate subsystem |
| Weak | The nearest reconstructed function references it |
| Very weak | The symbol is merely physically close to a function in the linked image |

No single weak signal should decide ownership.

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
pc=Unagi::InitResources+0x38
callers=Stage::Load -> Actor::Spawn -> Unagi::InitResources
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

For an Unagi-related candidate, contrasting experiments might include:

```text
Run A: Enter the stage without spawning Unagi
Run B: Approach and activate Unagi
Run C: Spawn and despawn Unagi repeatedly
Run D: Trigger another water enemy without Unagi present
Run E: Leave and reload the stage
```

The LLM can also recommend the next discriminating experiment. If two likely owners are `Unagi.cpp` and a shared water-enemy resource TU, running another water enemy without Unagi may separate the hypotheses.

## 10. Proposed ownership manifest

Inferred ownership should be recorded before definitions are moved. A machine-readable manifest can preserve the reasoning:

```yaml
symbol: data_ov016_02114d4c
module: ov016
address: 0x02114d4c
inferred_stride: 0x0c

candidate_owners:
  - tu: Unagi.cpp
    confidence: 0.91
    evidence:
      - constructed by __sinit_ov016_02114c20
      - constructor appears to be UnagiResources::UnagiResources
      - 8 of 9 static references belong to the Unagi TU cluster
      - runtime writes occur during Unagi::InitResources
      - runtime reads occur only while Unagi actors are active
    contradictions:
      - one reference from shared actor-resource cleanup

inferred_type:
  candidate: UnagiResources
  confidence: 0.78

status: proposed
```

Useful status categories include:

- `confirmed`: supported by surviving explicit metadata
- `high-confidence`: several independent signals agree
- `probable`: best current explanation, but alternatives remain
- `mechanical-only`: address and size are established; ownership is unknown

This format makes inferred claims reviewable instead of silently presenting them as facts.

## 11. Suggested inference workflow

### Phase 1: Establish hard safety gates

1. Wire `dsd check symbols --fail` into the ROM build.
2. Add a deliberately mis-sized BSS negative control and confirm that verification fails.
3. Ensure module image comparison, symbol-address verification, and section eligibility remain separate gates.

### Phase 2: Build the static evidence database

1. Parse all data and BSS symbols, addresses, sections, and implied strides.
2. Extract every relocation and code reference to each global.
3. Import reconstructed TU candidates from `tu_map` data.
4. Extract `__sinit` construction groups and inferred constructor types.
5. Record adjacency, module, overlay, vtable, registration, and resource relationships.

This can be modeled as a graph containing symbol, function, type, initializer, module, overlay, and candidate-TU nodes.

### Phase 3: Add targeted runtime tracing

1. Add address-range watch instrumentation to an emulator build.
2. Map PCs back to the decomp's function-symbol database.
3. Record active overlays and scenario metadata.
4. Produce compact per-symbol access summaries rather than sending raw traces directly to the LLM.

### Phase 4: Rank and review ownership

1. Generate focused evidence packets for a small symbol batch.
2. Ask the LLM for ranked owners, type hypotheses, contradictions, confidence, and a next experiment.
3. Review the first 50–100 symbols manually.
4. Measure agreement and identify consistently misleading signals.
5. Refine deterministic scores and LLM prompts before scaling.

### Phase 5: Drive source migration

1. Generalize `eligible.py`, `objisolate.py`, and the licensed-definition checks.
2. Teach enrollment to emit several declared section ranges per reconstructed TU.
3. Start with the mechanically safe one-symbol BSS pilot.
4. Gradually migrate high-confidence TU clusters.
5. Keep mechanically correct but historically uncertain definitions explicitly labeled.

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

## 13. Bottom line

The project currently rebuilds the game's instructions while copying almost all global state and constants from the original ROM. The underlying delinker already supports replacing those copied ranges with source-built sections. The primary blockers are repository safety tooling, missing sizes, compiler section behavior, and uncertainty about original translation-unit ownership.

An LLM-assisted approach is promising because ownership is a multi-signal inference problem. Static references, TU reconstruction, constructors, `__sinit` groups, runtime memory accesses, overlay context, and controlled experiments can be combined into explainable ownership rankings.

The hard linker rules must remain deterministic. The LLM should interpret semantics, rank candidates, identify contradictions, and propose additional experiments. Done this way, global-data migration becomes both a source-reconstruction project and an evidence-backed research pipeline rather than a mass relocation of `extern` declarations.
