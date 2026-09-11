# daSanbo_c source repair and composition, 2026-09-10

Producer session: `codex-r2445-0910`. This candidate continues the existing
`sanbo-ov096-0908-fakeiface` task and its offered checkpoint
`35a96691978f0251ca41fc0af87737e74a687cbf`. The companion reservation
`pr2447-provenance-composition-0910` owns the additional documentation and shared
composition files. The queue records the final immutable candidate and evidence
for both tasks. Active review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.

The composition includes main `cb5752cdf6b5d58f2b1f8a94bcbab2ed63749ac2`.
The renamed class header inherits main's corrected `void OnTurnIntoEgg` contract;
the TU definition now agrees. The old Pokey shard was modified on main after
this promotion deleted it. It remains retired, with its contract correction
carried into the promoted TU. The shared Actor contract is retained.

Three model-loading calls use `Model::LoadFile`, and two model-setup calls use
`mModel.SetFile`. Each replacement was first tested against the preserved
checkpoint. The composed TU verifies all 36 functions, 5,808 text bytes, with
zero blind words or differences; all 164 emitted symbol references resolve to
the ROM's permitted modules. Every function's bytes equal the freshly compiled
checkpoint object, including the corrected 64-byte OnTurnIntoEgg body.
Emitted metadata remains five VERIFIED and four PARTIAL records. This is a
text-only production entry; it owns no data or BSS.

The other interface probes remain limited findings. Passing `Fix12<int>`
objects to `dCcAc_c::Init`, using either aggregate initialization or explicit
field assignment, grew InitResources from 544 to 560 bytes. Calling the existing
scalar-parameter `dBgCh_Actr::Init` preserved masked instruction bytes but emitted
an unresolved external identity, giving strict BLIND-1. Neither variant is used.
These measurements justify retaining those two bridges in this candidate; they
do not prove that every typed implementation is impossible. The composed-tree
probes repeated the same two failures. Local scripts, objects and JSON reports
are preserved in the producer worktree and identified by the queue evidence.

The current provenance note now describes the previous-segment link, propagation
of root position through that link, and the reader and clear of `unk_3a8`.
It identifies the real Matrix4x3 member and existing matrix header. The dated
scout correction preserves original observations while superseding obsolete
writer instructions. The historical handoff and manifest now distinguish removed
render/PMF/link shadows, ten native method symbols, and the eleven-byte RTTI
name including its terminator. Address-derived helper names, C linkage and a
particular grouping of functions do not establish original source organization.

The inherited finding identities remain authoritative: `actor_virtual_shadows`,
`return_contract`, `return_contract_helper`, `render_shadow`, `flag_read_claim`,
`backlink_semantics`, `linkage_claim`, `typed_matrix`, `typed_subobject_calls`, and
`typed_links`. The review added `rtti_string_length`. The corrected source and
provenance findings require independent confirmation on the final candidate.

This remains partial reconstruction. Eight ordinary method definitions and the
inline destructor emit ten native method symbols; 24 address-named helpers and
two manual factories remain. Issue #2558 stays open for field/state-handler
reconstruction, supported interface conversions and factory experiments, owned
by the current review/integration run through @andrewboudreau.

Generated queue/state values and contributor ownership are checked on the
composed tree. Broader gate results belong to the exact candidate's queue
evidence, separate from the historical producer's proof figures. Before PR
publication or landing, an independent verifier must review both owned scopes
and the complete candidate. If target main advances, integrate and review that
exact composition again; this handoff does not authorize a stale-base merge.
