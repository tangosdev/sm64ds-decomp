# Handoff: issue-4570-message3-messageentry-shadow-1103

## Identity and resumption

- Task: `issue-4570-message3-messageentry-shadow-1103`; producer session `codex-source-audit-1103-next3`.
- Input/base and workflow pin: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`.
- Scope: the `MessageEntry` definition in `src/Message3.cpp`, consumer TU `arm9/Message3`, plus its dependency on `include/MessageBank.h`.
- Status: verified byte candidate; independent source review is the next queue stage.
- Next action: verifier should review that the existing MessageBank type is the correct shared record for the VS text consumer and accept or return this exact candidate.
- Remaining local-only material: ignored compiler reports and queue receipts under `build/`.

## What changed and why

- Added `MessageBank.h` to `Message3.cpp` and removed the file-scope duplicate `struct MessageEntry`.
- The header type preserves the same offsets for the 32-bit text offset, 16-bit width and line count; it names the final byte as `pad_07`, which the reader does not use. The same shared type is already used by the single-player `Message.cpp` reader.
- `Message3.cpp` reads only `lineCount` from this record. It does not use the local duplicate's signed `textOffset` spelling.
- No asset payload, runtime data, function body, field access or storage definition changed.
- `pad_layout_audit.py` on the pinned base reports 28 header shadows; on the candidate it reports 27. The `Message3.cpp` twin scan is empty after the change. Pad-member and padded-struct counts do not change.

## Proof

- Baseline before edit: `tubuild verify arm9/Message3 --no-write --verdict` passed 35/35; object isolation, relocation destinations and emission order were clean.
- Candidate command: `python tools/tubuild.py verify arm9/Message3 --no-write --verdict` passed 35/35; object isolation, relocation destinations and ROM-ascending emission order are clean. There are no unlicensed sections or symbols.
- `python tools/rombuild.py -j16 --no-rom`: `106/106` modules exact, `11,395/11,395` functions and `468/468` initialized-data claims reproduce, zero mismatches; symbol check passes.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or local header redeclarations.
- `python tools/source_style.py --base origin/main --files src/Message3.cpp`: zero findings.
- `python tools/find_shadow_types.py src/Message3.cpp --twins --format json`: no remaining local twin.
- `prepush_linkcheck` must be run after the candidate commit; it was not meaningful before commit because the range contained no changed files.
- Dead-reference and TU-conflict checks remain to run on the completed handoff.

The byte proof does not establish source ownership or semantics on its own; the verifier should confirm the reader and shared record agree as described above.
