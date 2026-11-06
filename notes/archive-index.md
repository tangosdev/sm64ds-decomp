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
