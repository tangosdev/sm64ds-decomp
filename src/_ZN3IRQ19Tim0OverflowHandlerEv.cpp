//cpp
// @symbol _ZN3IRQ19Tim0OverflowHandlerEv
//
// Fully migrated: the compiler mangles both the definition and the call. No
// hand-spelled mangled name remains in this file.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// The callee was renamed _ZN3IRQ13DmaTimHandlerEv -> ...Ej. "Ev" claimed void
// parameters, but the function reads r0 as an incoming argument
// (0x02056ccc `mul r4, r0, #0xc`; 0x02056cd8 `lsl r0, r0, #1`) and all eight
// callers pass an index 0..7. "Ej" (unsigned int) matches the declared u32 and
// every sibling in this namespace -- EnableIRQs, DisableIRQs, SetIRQs,
// GetIRQHandler, SetIRQHandler, ClearInterrupts, Restore, RestoreAll are all Ej.

namespace IRQ {

void DmaTimHandler(unsigned int irq);

void Tim0OverflowHandler()
{
    DmaTimHandler(4);
}

}
