# Wiring a worktree's gitignored inputs (and the one way to lose the ROM)

`git worktree add` checks out **tracked files only**. Everything the byte gates read is
gitignored, so a fresh worktree cannot compile or verify anything:

| path | what it is | size | replaceable? |
|---|---|---|---|
| `extracted/` | the ROM dump (`arm9_dec.bin`, `overlays/`, `dsd/config.yaml`) | 26 MB | **no** — needs a fresh cartridge dump |
| `tools/mwccarm/` | the pinned compiler + `license.dat` | 90 MB | proprietary, not redistributable |
| `tools/bin/` | `dsd.exe` | 8 MB | yes |

`tools/rombuild.py` and `tools/match.py` resolve all three as `REPO / ...`, where `REPO`
is the **worktree root**, not the main checkout. Nothing falls back, so a worktree with
these missing fails in confusing ways rather than obvious ones.

Link them in rather than copying — a copy costs ~124 MB per worktree and gives you a
second ROM dump to keep in sync. On Windows use a junction (`New-Item -ItemType
Junction`), which needs neither admin nor Developer Mode; a directory *symlink* needs
one or the other. On Linux/macOS a symlink is fine.

## The hazard: `git worktree remove` deletes THROUGH the link

Measured and reproduced twice on Windows junctions:

```text
decoy before: 2 files
git worktree remove --force
decoy after : 0 files          <- the link TARGET was emptied
```

git recurses into the reparse point and deletes the *target's contents*, leaving an
empty directory behind. On a wired worktree those targets are the real `extracted/` and
`tools/mwccarm/`, shared by every other worktree — and the ROM dump is not
re-downloadable.

**Always unlink the inputs before calling `git worktree remove`.** Tested and safe (they
do not follow junctions): `rm -rf`, and PowerShell `Remove-Item -Recurse -Force` in both
5.1 and 7.x. Only git's own removal is dangerous.

## Two more things that bite

**`build/` is per-worktree; `build/objcache` is safe to share.** Do not link all of
`build/` — it carries per-branch state that two worktrees will clobber, including
`eligible-names.txt` and the `rb_before.json`/`rb_after.json` pair. The eligible bracket
is a *before/after comparison*, so cross-contamination silently corrupts the verdict.
`build/objcache` alone is safe: keyed by content hash of source + deps + compiler +
flags, written atomically through `os.replace`, and `prune()` tolerates the races.
Measured payoff on a fresh worktree's first full build: **8,999 of 10,699 objects reused**
against a cache warmed by another checkout, **10,695 of 10,699** against one warmed on
the same commit.

**Keep the path short.** `git worktree remove` fails with `Filename too long` under deep
roots — `src/` holds ~11k files with mangled C++ basenames. It also fails
`Permission denied` if any shell still has the worktree as its cwd.

## Verify before you trust it

Path existence is not proof the compiler runs. Use a function known to reproduce under
the pin:

```python
import sys; sys.path.insert(0, 'tools')
import build_pin as B, pathlib
B.verify(pathlib.Path('src/unnamed/ov006/020c/func_ov006_020cb030.cpp'),
         'func_ov006_020cb030', 0x020cb030, 0x104, 'ov006')
# -> (True, '2004/b56')
```

**Do not use `pytest tools/test_build_pin.py` as the smoke test.** Its end-to-end cases
are gated on `_toolchain()` and *skip themselves* when the compiler or ROM is missing, so
an unwired worktree reports green.

## Hooks carry over, but gate less than you think

`core.hooksPath = tools/hooks` is repo-relative and `tools/hooks/` is tracked, so a
worktree gets the pre-push hook automatically. But link verification and attribution run
only for `refs/heads/main` — pushing a feature branch, the normal workstream, gets
neither. And `check_references.py` **skips** when no current eligibility report exists,
which in a fresh worktree it never does; it prints "skipping", not "passed".

So in a worktree the byte gates are yours to run, not the hook's.
