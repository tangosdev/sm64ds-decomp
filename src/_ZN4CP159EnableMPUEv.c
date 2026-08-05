// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (CP15/MPU coprocessor op), so there is no C to decompile it
// to -- the asm block is the faithful source. Counts as matched (asm-primitive
// policy), not a C transcription.
unsigned int _ZN4CP159EnableMPUEv(void){
    unsigned int v;
    asm { mrc p15,0,v,c1,c0,0 }
    v |= 1;
    asm { mcr p15,0,v,c1,c0,0 }
    return v;
}
