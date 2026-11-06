# Archived notes

The 2026-11-07 notes cleanup removed historical files from `notes/experiments/`
and `notes/agents/handoffs/`. Files that source, tools, config, workflows or
other notes still cite stayed in place; everything else is recoverable from the
annotated tag `archive/notes-pre-cleanup-1107`.

```sh
# list what was archived
git ls-tree -r --name-only archive/notes-pre-cleanup-1107 notes/experiments notes/agents/handoffs
# read one file
git show archive/notes-pre-cleanup-1107:<path>
# restore one file into the working tree
git checkout archive/notes-pre-cleanup-1107 -- <path>
```

Older retired notes live in [archive/ARCHIVE_LOG.md](archive/ARCHIVE_LOG.md).

## Class-note cleanup (branch 2)

A second pass removed finished or superseded per-class and per-subsystem notes
from the top level of `notes/`. Notes cited by `src/`, `include/`, `tools/`,
`config/`, workflows, `docs/`, `AGENTS.md`, or any kept note stayed in place.
They are recoverable from the annotated tag
`archive/notes-pre-cleanup-1107-classnotes`.

Removed: `class-init-pilot`, `collision-predicates-1031`,
`dScMgSingle3DBase_c-vtable-ownership`, `handoff-issue-2422`,
`handoff-validator-and-tests`, `match-attempts`, `match-provenance`,
`port-selftest-bmp-gate`, `rtti-name-audit`, `sinit-kurumajiku-interior-pointers`,
`sinit-ownership-wave-edges` (+ `-probe.cpp`), `sinit-ownership-wave-resource-handles`,
`tu-filename-reconstruction-pilot`.

```sh
git ls-tree -r --name-only archive/notes-pre-cleanup-1107-classnotes notes
git show archive/notes-pre-cleanup-1107-classnotes:notes/<file>
git checkout archive/notes-pre-cleanup-1107-classnotes -- notes/<file>
```
