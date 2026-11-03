# Handoff: issue-4570-fs-initfile-fsfile-shadow-1103

## Identity and resumption

- Task: `issue-4570-fs-initfile-fsfile-shadow-1103`; producer session `codex-source-audit-1103-next4`.
- Input/base and workflow pin: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`.
- Scope: the `FSFile` view in `src/FS_InitFile.c`, shared layout `include/private/fs_views.h`, function `FS_InitFile` at `arm9:0x0205d874..0x0205d89c`.
- Status: verified byte candidate; independent source review is the next queue stage.
- Next action: verifier should confirm the shared `FSFile` record is the appropriate SDK layout for this initializer and accept or return the exact commit.
- Remaining local-only material: ignored compiler reports and queue receipts under `build/`.

## What changed and why

- Replaced the partial local `FSFile` struct in `FS_InitFile.c` with `struct FSFile` from `fs_views.h`.
- The original view's fields had offsets `prev 0x00`, `next 0x04`, `arc 0x08`, `stat 0x0c`, `command 0x10`, `error 0x14`, and `queue 0x18`. The shared definition places the list links in `link.prev`/`link.next` at the same first two offsets and preserves every following offset.
- The shared record is 0x44 bytes; the local partial type was shorter. `FS_InitFile` receives an existing pointer and neither allocates nor sizes the record, so that difference does not affect its behavior.
- `pad_layout_audit.py` on the pinned base reports 28 header shadows; the candidate reports 27. Pad-member and padded-struct counts do not change.

## Proof

- Baseline and candidate `python tools/match.py --c src/FS_InitFile.c --func FS_InitFile --addr 0x0205d874 --size 0x28 --version 2004/b56`: all 10 instructions match the retail bytes; candidate bytes are unchanged.
- `python tools/rombuild.py -j16 --no-rom`: `106/106` modules exact, `11,395/11,395` functions and `468/468` initialized-data claims reproduce, zero mismatches; symbol check passes.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or local header redeclarations.
- `python tools/source_style.py --base origin/main --files src/FS_InitFile.c`: zero findings.
- `prepush_linkcheck` must be run after the candidate commit; it was not meaningful before commit because the range contained no changed files.
- Dead-reference check remains to run on this completed handoff.

Byte identity does not establish source ownership by itself; the independent reviewer should confirm the layout reasoning and usage boundary above.
