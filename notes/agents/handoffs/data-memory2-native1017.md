# Memory2 native data constants

Task: `data-memory2-native1017`; issue [#4020](https://github.com/tangosdev/sm64ds-decomp/issues/4020).
Producer: `codex-memory2-native1017`. Source base: `e8dc126bcda1865cce7a4a4cb920fb2095deefb9`.
Tested dependency base: `2801e012eea47be85d78732b81ff32c3df84e2fb`;
the source-only PR targets `refs/heads/tools/elf-symbol-name-growth` (PR #4024).
The tool code was independently accepted at `9fb6d6a394df537069d62f8a5a394b3c3c7b313f`;
the later commit changes only its handoff's branch reference. The dependency has
its own accepted queue task, `tools-elf-longnames1017`.

The existing five dispatch tables now initialize from native pointers to
`dScMgMemory2_c` members. The pinned compiler emits their 29 eight-byte constants;
the manifest identifies each compiler-local literal by its exact method relocation
and binds it to the existing `pmf_dScMgMemory2_c_*` symbol. An existing
`BaseSpawnInfo<void>` aggregate supplies the interleaved eight-byte profile.
The newly claimed `.data` interval is `ov006:0x0213d248..0x0213d338`.

The existing method declarations are retained. `RoundWaitDeal` currently declares
a `void*` return; its explicit member-pointer conversion retains the existing
void-return dispatch table, whose caller discards that result. Exact pinned-ABI
behavior does not establish the original return signature. That source-signature
reconstruction is deferred on issue #4020, owned by `codex-memory2-native1017`;
the issue must remain open after this data slice. No free-function declaration of
a mangled member, raw code-pointer structure, or shared-header edit is introduced.

PR [#3937](https://github.com/tangosdev/sm64ds-decomp/pull/3937), DD103 commit
`c64fad8cd903f6c806ad5649be258cff707168d1`, preserved the 240-byte candidate and
its object boundaries. This continuation retains that evidence and replaces the
candidate's raw member representation with native compiler emission. Existing
function-author attribution is unchanged. Current descriptive sprite/table names
are preserved rather than replaced with the older candidate's address labels.

The compiler-object comparison checks every emitted function, including the
initializer and compiler helpers: 56 functions retain identical bytes and
canonical relocation destinations, types and addends. All six existing BSS
objects retain their sizes and layout. The initializer remains 956 bytes with
36 relocations. The 29 relocated native literals and profile each reproduce their
complete eight-byte retail record. These object checks are separate from final
production-provider acceptance.

Producer validation passed in the private wired worktree. Normal TU scratch
linking reproduces all five licensed sections and the full 16 MiB ROM, SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The stock reference was independently hash-checked before installation; it is not
a candidate output relabeled as a baseline. The strict symbol command still has
nine baseline errors and zero new errors; it is not reported as clean.

The fresh default production build (`rombuild.py -j16 --no-rom`) passes all
106 modules, all 11,334 source-built functions and all 339 complete initialized
data claims. A before/after-link provider audit verifies all 30 new data records
and all six existing BSS objects from this TU's compiled object, with no ROM-gap
provider for those requested extents. The audit binds the wrapper, source/config,
actual tool paths, generated linker configuration and link inputs. It uses the
external PR #4015 tooling at `b045eaba5fdc50e097a78be7f7ac55e0da308816`;
the production compiler/linker tools come from this worktree. The temporary
wrapper uses a Windows-safe `__main__` guard. This no-ROM production invocation
does not claim to regenerate the installed reference; full-ROM proof comes from
the separate scratch build above.

Focused `tubuild verify` is 52/52 with correct relocation destinations and order.
Its raw-object inventory still refuses the existing initializer in `.text`;
normal TU preparation places it in its licensed `.init` range, verified above.
Declaration and dead-reference checks pass. The optional broad ROM-data report
still shows the unrelated `_ZTV10dCcAcPos_c` difference in its two old destructor
objects; every requested Memory2 record passes the fresh provider audit.

The verifier must independently review this source and accepted tool dependency,
rerun the complete data-range/initializer gates and publish immutable evidence.
Producer proof is not independent acceptance. The compact committed record is
`notes/experiments/data-memory2-native1017.json`; full reports and the wrapper are
ignored artifacts under `build/` in the producer worktree.

Remaining scope is explicit: the shared sprite table and neighboring RTTI/data
outside the 240-byte interval are not newly claimed. This is a coherent production
data contribution, not a claim that the entire minigame data backlog is complete.

The metadata follow-up resolves `memory2-verification-summary-001`: the current
raw-verification criterion incorrectly retained an older, longer failure message.
It was regenerated to report one existing initializer classification refusal and
zero compiler-only policy errors. The source, delinks and symbol registry are
byte-identical to tested commit `6307fbb9a852e30eb786785adf97f6123c7b7384`;
every other manifest value, including historical link receipts, is unchanged.
The full-ROM, production and provider proofs above belong to that exact predecessor.
Independent verification of this follow-up must confirm functional input identity
and the corrected diagnostic, rather than relabel those old receipts as new runs.
Deferred finding `memory2-return-signature-001` remains open on issue #4020.
