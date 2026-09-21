# ApplyOpacity interface reconstruction

This changes the shared method to `void ModelBase::ApplyOpacity(u32 opacity,
u32 /* unused */)`. The method still passes the embedded model-components view and
opacity to the same retail implementation. The added parameter is unused.

The old one-word mangling was already a reconstructed label in the initial
public symbol table; converting that bridge to a real method in #981 preserved
its spelling. Neither ApplyOpacity nor 9ModelBase occurs as a stock-image string.
The committed `batch3-2707-tree-applyopacity-census.md` records all 25 retail
call sites setting the second scalar word (15 zero, 10 one). This supports a
two-scalar reconstruction. It does not recover the original signedness, name,
or meaning of the unused parameter; u32 and `unused` are explicit choices.

At input50255659, independently measured one-word native/bridge calls in
daMip_c::RenderMirrorImage both remove `mov r2,#0` at0212c1dc and shrink the
function from0xe0 to0xdc. They do not reproduce retail. The coherent two-word
API retains that argument naturally without a wrapper or register padding.

The canonical symbol is now _ZN9ModelBase12ApplyOpacityEjj at unchanged
arm9:02016a9c/0x10. Every production/shadow caller uses that same interface;
existing typed receivers call the member directly. The scalar forwarding helper
func020461b4 also uses the actual unsigned opacity consumed by func02046208.
All34 old arity/mangling baseline records and its one proven scalar forwarding
parameter disagreement are removed; no new disagreement is banked. The callee's
original ruspecial credit and converted identity are preserved through the rename.

The user explicitly authorized this PR-specific API patch, including the reserved
ARM9 entries. Existing owner branches, leases and reservations remain intact;
unrelated configuration and ledger records are preserved. The approved patch
SHA256 is `d65fee7c9be670e364db24524856a1051a4fe4efd5344c8405a9c60bf1bb1c6a`. Mirrored-input proof is recorded separately;
the full header-consumer gate, current-main production ROM, final static gates,
and independent formal source review remain required before acceptance.

The unused second parameter is unnamed in both declaration and definition.
This ordinary spelling preserves the existing compiler-local identities in
other translation units. Independent controls verify all27 complete objects
against current main after only the canonical ApplyOpacity symbol rename;
no manifest or overlay-symbol renumbering is part of the final change.
