# Minigame production promotions

Base: `f2957e08dec8afcf21e0e44637e6311bffb7833d` (fetched remote main).
Worktree: `C:/tmp/sm64ds-mg-finish-0930`; branch: `cpp/minigame-promotions-finish-0930`.
Follow-up: https://github.com/tangosdev/sm64ds-decomp/issues/3317.

## Production ownership

| Candidate | Functions | Production source | Claimed range |
| --- | ---: | --- | --- |
| dMgState | 4 | src/minigames/d_mg_state.cpp | ov004 text 020b8714..020b8a8c, BSS 020bfd04..020bfda8 |
| Slot1/Slot3 | 39 | src/minigames/d_s_mg_slots.cpp | ov006 text 0210a8c0..0210c9e0 |
| Smartball scene and objects | 132 | src/minigames/d_s_mg_smartball.cpp | ov006 text 0210d740..02119904 |

All three are `complete` production entries. The 157 absorbed source files and three shadow paths are retired; the old 18-function Slot1 manifest is superseded by the combined 39-function entry. Slot1 InitResources was independently relocation-verified and enrolled; its factory remains enrolled separately.

Slot1 Behavior is deliberately outside the claim: its existing 0x81c-byte NONMATCHING draft at 0210c9e0 retains the documented 19-word touch-coordinate mismatch (notes/mwccarm-codegen.md section 6cm). It remains a retail gap. These promotions do not claim complete class reconstruction: manual factories, opaque layouts, and address-named helpers remain in Slot/Smartball and are tracked in issue 3317.

## Source and linker decisions

- dMgState retains its normal function-local callback table. The consolidated compiler names its guard/table with `$26`; the old `$19` shard names and generic aliases are retired atomically. Only two SetState relocations reference that storage. Explicit local-to-global ELF binding policy changes visibility, not storage or source lifetime.
- The ELF binding helper needed a separate repair: update `.symtab.sh_info` after promoting trailing locals, and reject policies that would interleave locals and nonlocals. Focused regression tests prove symbol-index, relocation, and payload preservation. This tooling is a separate commit from the promotions.
- Existing strict-control support independently demotes exactly owned intact objects to retail gaps. Candidate admission discards its initially prepared object, recompiles raw source, and audits it before linking. No proof or byte gate was bypassed.
- Slot/Smartball use `defer_codegen off` with balanced per-function optimizer scopes and ROM-order definitions. Smartball restores the legacy scalar load and explicit signed parameter conversions. Slot3 uses its actual class state and member-pointer dispatch.
- Full emitted metadata payloads were independently compared: Slot 21 records / 552 bytes; Smartball 47 records, including all 17 complete RTTI strings. Zero differences or blind words. Text-only production imports this metadata from configured ROM homes; it does not claim to own those data ranges.

`tu_promote.py` performed the production moves and enrollment. Its batch stopped when Git refused to remove the locally corrected Slot3 initializer. That exact repair was retained in ignored build evidence, then its verified replacement retired it and the same helper attribution/converted-identity routines completed the batch. The superseded Slot18 manifest was explicitly removed. This is disclosed recovery of an interrupted mechanical operation, not a different enrollment policy.

## Credits and declaration accounting

An independent address-keyed `validate_merge` snapshot audit covers all 175 members. The mechanical helper missed 18 members from the already-consolidated Slot1 source and two Smartball factory-file members; explicit `path#symbol` overrides restore their exact original authors. Unrelated credit mappings are preserved.

Declaration-baseline migration carries 40 exact existing fingerprints to the new source paths and retires 213 fingerprints on deleted paths. Every carried fingerprint is evidenced in an absorbed source or a shared header it included; shared-header records remain. Six unused conflicting declarations and three redundant local declarations were removed. No new declaration exception was invented, and unrelated baseline entries were retained. The check reports no new disagreements or header redeclarations.

## Validation and handoff

- Pinned compiler: 2004/b56.
- dMgState: 4/4 text matches; independent intact-object link proof is SCRATCH-DATA-VERIFIED, including BSS ownership and 0 new symbol errors.
- Slot: 39/39 matches; Smartball: 132/132 matches. Object isolation, relocation destinations, and emitted order are clean.
- Combined production build: 106/106 modules exact; 11,219 source-built functions reproduce, 0 mismatching; full 16 MiB ROM SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, identical to stock.
- DSD's symbol check still reports nine pre-existing baseline errors. It is not globally green; this candidate adds zero errors.
- Port references: 408 checked, all resolve. Declaration agreement: no new disagreements or header redeclarations.
- ELF focused regression tests pass. Broader tests have four independently reproduced pre-existing pilot-fixture failures; no production gate is relaxed for them.

Exact-commit production, attribution, relocation, and independent source-review results are attached to the queue handoff after committing this candidate. The source-review completion classification is partial because of the explicitly retained reconstruction work above. No merge is authorized by this handoff.

## Coordination

The user explicitly authorized releasing the missing-worktree reservations `pr2877-interface-repair-0921` and `promote-puzzle-state-0928`. Their history and artifacts were preserved while their reservations were cancelled. Replacement task `mg-promotions-finish-0930` owns this work; global per-symbol attribution and declaration/converted identities were reconciled in the integration lane without changing unrelated owners' mappings. Wired worktrees and the earlier byte-verified Moneybag commit remain preserved. The primary dirty checkout was not edited.

## Independent review repairs

The final review corrected obsolete Smartball history and qualified the destructor codegen observation to the pinned build. Slot3's layout comment now identifies its retired Render struct as legacy evidence.

Cup Render now indexes the existing `mFrame[k]` field. The old comment claiming that spelling changed code was unsupported: before and after objects are identical across all 22,056 bytes (SHA-256 `777dbb9b72e9e30a39ad032bc99a0c1f4e746a3a7f3eaeb7d29b61cb819525ab`). The remaining x/y spellings are unchanged. This repairs inherited finding SCMG2877-01 in the current source.

Fresh RTTI, vtable, and TU-map extraction fed `queue_audit.Tree.measure` for the affected queue rows. State uses the verified intact-object route; Slot's combined row records its separately enrolled tail and unmatched Behavior. The obsolete standalone Slot1 row was removed: the mapper class label at 02119824..02119904 covers Smartball's factory and callback, both explicitly owned by the Smartball manifest. The Smartball family row retains `already_promoted=no` under the checker’s per-class-manifest semantics, with an explicit production-owner note covering all 132 functions. Luigi, Memory2, Flower, and Cup line counts and pragma-file counts were refreshed; unrelated rows were preserved. This corrects inherited LUI2876-05 and MEM2875-05 evidence to the current tree.

The additional missing-worktree reservation `pr2500-cup-source-repair-0911` was cancelled under the user's unlock instruction, preserving its history. Its missing worktree was `C:/tmp/sm64ds-vcup-r2398-0911`. The replacement task reserved Cup's Render repair and queue accounting before applying them. Cancellation releases a reservation; it does not certify that old verifier's unfinished review.
