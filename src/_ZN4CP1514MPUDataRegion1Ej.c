// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (CP15/MPU coprocessor op), so there is no C to decompile it
// to -- the asm block is the faithful source. Counts as matched (asm-primitive
// policy), not a C transcription.
void _ZN4CP1514MPUDataRegion1Ej(unsigned int x){
    asm { mcr p15,0,x,c6,c1,0 }
}
