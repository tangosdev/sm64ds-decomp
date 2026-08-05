// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (compiler trap builtin), so there is no C to decompile it to
// -- the asm block is the faithful source. Counts as matched (asm-primitive
// policy), not a C transcription.
void _ZN4cstd14__builtin_trapEv(void){
  asm { dcd 0xe7ffffff }
}
