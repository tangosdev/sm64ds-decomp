# Agent guide: migrate SM64DS classes to real C++ in parallel

This is the operating guide for several AI agents working toward source files that
look like the original Nintendo EAD C++ translation units, without weakening the
decompilation's byte and relocation guarantees.

The immediate job is to replace fake C++ such as a hand-written function named
`_ZN4HeapD1Ev` with compiler-spelled C++ such as `Heap::~Heap()`. The eventual job is
to group methods that evidence places in one original translation unit. Those are two
separate proof steps; do not guess the second from the first.

Read `AGENTS.md` first. Prefer tangOS Console when it is available. When driving the
repository by hand, also read:

- `notes/runbook-type-reconstruction.md`
- `notes/dtor-migration.md`
- `notes/plan-cpp-language-mode.md`
- `.claude/skills/decomp-cpp-class-form/SKILL.md`
- `.claude/skills/decomp-tu-build/SKILL.md`
- `.claude/skills/decomp-tu-slicing/SKILL.md`
- `.claude/skills/agent-lock/SKILL.md`

## Non-negotiable result

A migration is real only when all of these are true:

1. The C++ compiler spells the class, namespace, method, constructor, or destructor
   symbol. The source does not define its own `_ZN...` name.
2. The licensed function reproduces the ROM bytes with the pinned compiler.
3. Every relocation resolves to the ROM's actual destination, not merely a symbol of
   the same shape.
4. Any emitted vtable, RTTI, guard, static, thunk, constructor variant, or destructor
   variant has been inspected directly. `objisolate` can discard unlicensed sections,
   so an isolated function match does not prove the discarded metadata is correct.
5. A full repository relink is exact, affected header consumers remain exact, and the
   repository's attribution, reference, offset, port, and language-mode gates pass.

Near-misses never belong in `src/`. Restore the original matched source and bank the
candidate in `nearmiss/db.jsonl` as described in `AGENTS.md`.

## How to split the work

Give one producer agent ownership of one class family, not an arbitrary list of
symbols. A class-family lane includes:

- the class header and direct base-class declarations;
- all remaining fake-C++ methods for the class;
- its constructor/destructor variants;
- its vtable and RTTI address range;
- the module's `symbols.txt` and `delinks.txt` entries;
- every source reported by `tools/affected_src.py` when a shared header changes.

Keep address ranges and shared files disjoint across lanes. Good parallel lanes are
unrelated classes in different overlays or unrelated arm9 ranges. Do not split D0,
D1, D2, the key function, and vtable ownership among different agents.

Use a second agent as a read-only verifier after the producer commits. The verifier
must inspect raw emitted metadata as well as re-running executable gates. A producer's
own green output is evidence, not independent review.

## Coordinate locally with Redis

Remote claims prevent duplication across contributors. The local Redis lock prevents
agents on this machine from editing the same files or address ranges. Use both.

Start Redis if needed:

```powershell
docker compose -f local-infra/docker-compose.yml up -d
```

Every agent needs a unique holder name:

```powershell
$env:AGENTLOCK_HOLDER = 'cpp-heap'
python tools/agentlock.py list
python tools/agentlock.py acquire `
  --files include/Heap.h src/_ZN4HeapD0Ev.c src/_ZN4HeapD1Ev.c `
          config/arm9/symbols.txt config/arm9/delinks.txt `
  --range arm9 0x0203ca10 0x0203ca54 `
  --ttl 3600 --wait 60 --note 'real C++ Heap migration'
```

Acquire the complete resource set atomically before dispatching the lane. Renew long
jobs, and release only after the branch is pushed or deliberately abandoned:

```powershell
python tools/agentlock.py renew `
  --files include/Heap.h src/_ZN4HeapD0Ev.c src/_ZN4HeapD1Ev.c `
          config/arm9/symbols.txt config/arm9/delinks.txt `
  --range arm9 0x0203ca10 0x0203ca54

python tools/agentlock.py release `
  --files include/Heap.h src/_ZN4HeapD0Ev.c src/_ZN4HeapD1Ev.c `
          config/arm9/symbols.txt config/arm9/delinks.txt `
  --range arm9 0x0203ca10 0x0203ca54
```

Use the actual files and exact half-open address span for the selected class. The
Heap names above illustrate the shape; they are not a reusable lock list.

## Producer workflow

1. Fetch `origin/main`, create a short-path wired worktree with the
   `decomp-worktree` skill, and verify the canary before editing.
2. Reserve the remote claim and the local Redis resources.
3. Run `python tools/langmode_audit.py --by-class` and select a header-backed class.
4. Inventory every class symbol and its ROM range. Inspect the base class, vtable,
   RTTI, destructor slots, and known callers before changing a declaration.
5. Ask `python tools/srcpath.py <symbol>` for every existing path. Do not invent paths.
6. Change one candidate at a time to compiler-spelled C++. Keep only candidates that
   pass strict bytes and relocations. Restore or bank every failed probe.
7. If a key function emits a vtable group, inspect the complete object before relying
   on isolation. Compare section sizes, symbol names, slot order, relocation targets,
   RTTI kind, base RTTI pointer, and address-point convention to the ROM.
8. Re-run the whole class family after the header reaches its final form.
9. Commit only a coherent verified class slice. Do not mix tools or documentation into
   a source migration PR.

The per-function strict check is:

```powershell
python tools/linkcheck.py --c <source> --name <symbol> `
  --addr 0x<address> --size 0x<size> --module <module>
```

Use the arguments required by the current tool version and require `VERIFIED` with
`blind: 0`. Then run the repository gates named by `AGENTS.md`, including affected
consumers for every changed header and a full ROM build.

## Direct metadata check

Do not stop after `linkcheck` and `rombuild`. Compile the unisolated C++ object and
inspect its symbols, sections, and relocations with the repository's compiler/object
tools. Answer these questions explicitly:

- Which translation unit owns `_ZTV`, `_ZTI`, and `_ZTS`?
- Does the emitted table include the ABI preamble before the address point?
- Are all slots present, in ROM order, with the correct destructor variants?
- Does derived RTTI point at the correct base RTTI, and is the RTTI structure kind
  correct for single or multiple inheritance?
- Did the compiler emit extra C1/C2/C3 or D0/D1/D2 bodies?
- If isolation drops those sections, were they nevertheless correct?

If any answer is unknown, the slice is not ready.

## Bringing one class into one file

Class-family ownership comes first; physical translation-unit consolidation comes
later. While functions are being converted to real C++, retain the repository's
one-symbol-per-file enrollment so each contribution remains independently licensed and
verified.

Promote several methods into one `.cpp` only when translation-unit evidence supports
that grouping and the TU tools pass their known-answer checks. Before trusting a TU
map, generate its prerequisites in order and require its check to report success:

```powershell
python tools/rtti_extract.py
python tools/rtti_vtables.py
python tools/tu_map.py
python tools/tu_map.py --check
```

Then follow `decomp-tu-slicing` and `decomp-tu-build`. A real TU promotion must prove
the whole compiled object: text contributions, data/rodata/bss, vtables, RTTI, static
initialization, relocations, padding, ordering, and all linked consumers. A plausible
class grouping or adjacent addresses are not enough.

## Independent verifier checklist

The verifier works read-only from the producer's committed branch and returns either
`READY` or `BLOCKED`.

- Review the complete diff against the current `origin/main`.
- Confirm every migrated definition is compiler-spelled.
- Re-run strict `linkcheck` for every licensed symbol.
- Inspect the unisolated object metadata and compare it to the ROM.
- Run `tools/affected_src.py` for changed headers and verify the consumer closure.
- Run the full ROM/link build and require every module exact.
- Run offset, attribution, language-mode, dead-reference, and `port_refcheck` gates.
- Confirm there are no near-misses, unrelated edits, stale comments, or untracked
  generated artifacts in the commit.

Only after `READY` should the producer branch be pushed and a narrowly titled source
PR opened. Release the Redis lock after publication, refill the lane with the next
disjoint class, and continue until the audit has no migratable backlog or every
remaining item has a documented compiler/ABI wall with its best candidate banked.

## Prompt for a producer agent

```text
Own the <Class> real-C++ migration in <worktree>. Read AGENTS.md and the class-form
skill. Respect the existing Redis locks. Inventory the whole class family, then replace
hand-spelled mangled definitions with genuine compiler-spelled C++ one candidate at a
time. Preserve exact bytes and relocation destinations. Directly inspect every emitted
vtable/RTTI/constructor/destructor variant so objisolate cannot hide bad metadata.
Restore or bank near-misses; never leave them in src. Run affected-consumer and full
repository gates, commit locally, and report exact evidence. Do not push or open a PR.
```

## Prompt for a verifier agent

```text
Audit <branch/worktree> read-only against current origin/main. Do not edit. Confirm the
definitions are compiler-spelled, rerun strict byte and relocation checks, and inspect
the raw unisolated vtable/RTTI/variant metadata rather than trusting objisolate. Verify
the changed-header consumer closure, full ROM/link, offsets, attribution, language-mode,
port references, dead references, and diff hygiene. Return READY or BLOCKED with exact
evidence and name every unresolved assumption.
```
