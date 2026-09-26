# src_tu inventory test repair

Producer: codex-independent-review0926. Queue task: src-tu-inventory-test-repair-0926.
Base: 6d4f8128d585356092a4e656155daed881101cd8. This tooling-only continuation supports PR3199 and reserves only tools/test_check_src_tu.py and this handoff.

PR3199 at 3df8eedd69f3499067e1777e7ee588c0f714befb validly reduces the shadow source inventory to ten files. The reference gate passes, but the old committed-tree test requires more than ten and fails one of sixteen tests. The separate reference-count floor would cause the same problem after further valid promotions.

The repaired test obtains an independent C/C++ source inventory with Git ls-files, requires it to be nonempty and present, and requires the checker source count to equal that inventory. Reference work must remain nonempty. tools/check_src_tu.py is unchanged; all sixteen existing tests remain, including empty TU/include/reference worklists, failing CLI status, stale includes/symbols and unknown compiler-emitted class names.

Validation: all sixteen tests and the actual gate pass on the eleven-source main base and with only the repaired test overlaid onto PR3199's ten-source tree. The original sixteen-test suite was reproduced on the exact PR3199 tree and failed only the obsolete numeric floor. Independent counter perturbations leave the gate verdict green but lower checked sources by one: the repaired test rejects both 10-of-11 and 9-of-10 walks through the inventory equality assertion. Private evidence is in the producer worktree build directory and the separate wired src-tu-inventory-case3199-0926 worktree. No source, header, manifest, ROM config or gate implementation changes are part of this PR; no new ROM claim is made.

Root independently reviews and integrates this tooling candidate. After it lands, refresh PR3199's validation against the updated merge base. Do not merge this supporting tool change into the source PR's implementation scope.
