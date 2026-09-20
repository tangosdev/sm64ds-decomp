# PR 2386 tooling repair

This document describes this commit. The queue records the immutable output SHA.
Task: pr2386-tool-repair-0920; producer: codex-draft-tools-0920 (Codex).
Input: b8d41569f9d5a84be59554bd36d0ac56e4878a97, branch review/drafts2386-0919.
Original source base: 2996608719a316a98d0c5da24ff90e45b00ac550.
Active workflow policy: 18af52b49b9fb5663d417bdfa86109b7b157aaa3.
The old branch's embedded workflow documents are historical inputs, not the active
policy used to claim this repair. The primary checkout supplied the active queue tool.
PR: https://github.com/tangosdev/sm64ds-decomp/pull/2386

## Scope

Nine reserved documentation/tool/test files only. No source, headers, baselines,
AGENTS policy, attribution, or current-main integration changes. The coordinator
added the registry reservation solely for this handoff's evidence row.

The index counts UTF-8 bytes after newline normalization, including its own text.
The existing fixed-point writer remains responsible for its self-counting budget.
The benchmark uses the same normalized byte convention and visits equal-hop
neighbors in lexical order after the declared front-door order. It measures a
path-byte proxy for ten questions, not reader time, model tokens, or a general
67 percent cost improvement.

The reference scanner treats only the port-linkage stamp's top-level string
branch and basis fields as historical Git provenance. It still checks comments,
unknown fields, nested values and malformed JSON. Plus signs remain part of whole
path tokens, including in directory names. No new baseline exemption was added.

## Verification

All checks below ran locally on this repair's tools and original draft tree.
Independent review belongs to the coordinator.

- `python -m unittest discover -s tools -p test_notes_*.py -v`: 12 tests pass.
  Fixtures cover LF/CRLF and multibyte UTF-8 accounting, README fixed points,
  stale content, missing/unregistered notes, deterministic equal-hop routes,
  preserved shorter paths, and missing/changed answer facts.
- `python tools/test_check_dead_references.py`: 54 tests pass, including all
  existing controls and six new schema/path tests. Historical string fields are
  excluded only in the exact stamp; missing comment paths, unknown/nested fields,
  malformed input and ordinary notes and JSON still report failures. Escaped newline
  text remains genuine comment prose. Live plus-containing paths resolve and
  missing ones are reported under their complete names.
- `python tools/notes_index.py --write`, then `--check`, then a second `--write`
  and `--check`: all exit 0, second write byte-identical. The generated index
  covers 156 files; normalized doctrine is 364,788 bytes across 19 files.
- `python tools/notes_bench.py --strict --baseline <local JSON>` under
  `PYTHONHASHSEED=1` and `8675309`: both exit 0 with identical results. All ten
  questions resolve and are reachable in one hop; summed path bytes are 393,760.
  These normalized figures cannot establish the original headline's unmeasured
  general 67 percent cost reduction.
- `python tools/check_dead_references.py`: exit 0, no new dead references or
  broken Markdown links. Existing baselined debt remains; neither baseline was
  edited. Final scan population is recorded with the local evidence.
- Build-only scanner probe of the exact port-linkage JSON from main
  05f525ac1c577b151566565cbe42bfc8726bca32: historical ref tokens are excluded;
  actual comment paths remain present. This is a fixture check, not acceptance
  of a composed current-main tree.

Logs and JSON are under the private worktree's ignored build directory with the
pr2386 prefix: notes-tests, deadref-tests, index-idempotence, bench-seed1,
bench-seed8675309, deadrefs-final and current-port-probe. The producer report pins
this commit after it is created; this handoff does not embed its own future SHA.

## Remaining work

The PR remains draft. This repair preserves original ancestry and does not resolve
its current-main documentation, policy, and retired-source conflicts. Reconcile
those separately without replacing newer research or reviving retired code.
Full ROM, relocation checks and private validation were not run for this tooling
repair. No C++ source or compiler input changed; no ROM acceptance is claimed.
Local evidence is retained in the worktree build directory. Nothing was pushed or
published by this producer. Root owns review and any authorized draft publication.
