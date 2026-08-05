// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (CP15 DTCM base read: an mrc p15 coprocessor op no C construct
// compiles to), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
unsigned int _ZN4CP1518GetDTCMBaseAddressEv(void){ asm { mrc p15,0,r0,c9,c1,0; ldr r1,=0xfffff000; and r0,r0,r1 } }
