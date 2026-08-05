// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (CP15/MPU coprocessor op), so there is no C to decompile it
// to -- the asm block is the faithful source. Counts as matched (asm-primitive
// policy), not a C transcription.
void _ZN4CP1516WaitForInterruptEv(void){
    unsigned int v = 0;
    asm { mcr p15,0,v,c7,c0,4 }
}
