# Koopa composition after Mip Key landed

This commit composes the accepted Koopa repair `63c0a9605ce0742462ca4fdf2c4570aa56240043`
with main `c04f97677c3c30b269354a36435c027bdd0afd5f`, which landed Mip Key PR #2853.
The original accepted branch and both source histories are preserved.

The Koopa source, header, manifest, dMeter_c and Timer initializer are identical to the
accepted repair. The Mip source, header and manifest are identical to main.
No new source reconstruction or compiler-constraint claim is introduced here.

The only conflicts were adjacent ledger records. Both actors' complete credit
sets remain: 18 Koopa entries and 15 Mip entries. All three conflicting legacy
declaration views were already retired by their respective accepted promotions;
their deletion sets are combined. The whole parsed ledgers equal an independent
three-way reconciliation. Converted accounting retains exactly 2,973 identities,
including the three Koopa and five Mip rekeys, with no count increase.

The user's narrow PR-specific attribution/declaration authorization still applies.
The existing owners and their reservations are preserved. The integrator owns
the converted-ledger and this handoff reservation through pr2859-after-mip-0920.

Keep RNK-01 through RNK-08 and the partial reconstruction under
[issue #2866](https://github.com/tangosdev/sm64ds-decomp/issues/2866), next owner
codex-integrator-0920. The two existing, unchanged and unenrolled consumer warnings
remain explicit coverage limits; matching main's retail fallback does not close them.

Current-composition gate results and the independent exact-head/base review are
recorded separately in the queue evidence. This handoff does not itself assert
that those subsequent checks, original-branch publication or merge have occurred.
