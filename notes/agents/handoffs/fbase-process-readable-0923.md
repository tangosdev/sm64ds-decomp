# Root actor process readability

Issue: https://github.com/tangosdev/sm64ds-decomp/issues/3012. Task: `fbase-process-readable-0923`. Input/base: `07bb515b4c8b1022b013ee68a0fbca10632ed003`. This commit is a small production-source improvement; no source relocation or TU promotion is involved.

`Process` is now a nonvirtual `fBase_c` method with named action/before/after callbacks instead of a fake receiver class and hand-mangled definition. The three typedefs preserve the currently configured int/bool/u32 signature. Its call order, return value and after-status mapping are unchanged. Initialization uses the existing manager nodes and byte members, and the parent accessor follows the real scene-node fields.

The complete 9,912-byte ActorBase object is identical to the input under `2004/b56` (SHA256 `8be6d52725c9571189b140b313e5b79bb07b6dd1a75fc674affc64b38e68c583`), including all 25 licensed functions, compiler-only output and relocations. Producer and independent verifier publish final consumer, strict-link, whole-object, metadata and full-ROM evidence in the queue, bound to exact candidate/base commits.

Source review fixes remove an unsupported polymorphic-member-pointer barrier, stale migration/skill instructions, an obsolete claim that AfterCleanupResources is unenrolled, and incorrect descriptions of successful default hooks as VS_FAIL. The allocator comment now distinguishes the global default-heap delete at arm9 `0x0203cbf0` from the actor-heap call, addressing #2432 without changing either allocator. Independent review also corrected the cleanup-child condition, deleting-destructor instruction-count claim, and distinction between the unsigned allocation bridge and size_t forwarding overload.

## Deliberate limits

- `FBASE-CALLBACK-CONTRACT-01`: partial reconstruction. The configured Process symbol uses a bool guard, but cleanup/behavior/render guard families still declare int returns. The four existing C wrappers pass ROM ABI records and remain unchanged; ordinary typed calls to those three families are not yet supported. Their virtual records and full-word condition do not prove an original bool return type. Reconcile the entire override/caller family in a later task under #3012; owner: `codex-root-fbase-readable-0923`. No casts or adapters hide this boundary.
- `FBASE-RECONSTRUCTION-01`: partial reconstruction. The lifecycle-list maintainer, heap helpers, explicit destructor variants and key-function/data ownership remain as inherited. The separate Manager deadstrip question remains #2025. These are not declared fully reconstructed by this slice; further root-class work belongs to #3012 with the same owner.
- Four simple early-return/boolean-expression trials for cleanup, behavior, render and destruction guards changed the emitted object (cleanup/destruction also shortened it by 16 bytes). They were reverted. This is a bounded failed experiment, not a claim that cleaner exact forms are impossible. Local trial sources/objects and results are in `build/basecore/lifecycle-probes.json`; no retained code depends on them.

No layout, virtual-slot, symbol, enrollment, baseline or attribution change is made. Independent review precedes a focused PR; main merge is not authorized by this handoff.
