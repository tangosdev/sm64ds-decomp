/* SPLEmitter_Emit @ 0x204a5bc (arm9) -- tail-call veneer to SPLEmitter_EmitParticles (0x204c584).
 * ldr ip, [pc]; bx ip; .word 0x204c584
 *
 * Forwards both of its arguments in r0 and r1. A tail call leaves them alone,
 * so the old (void) spelling matched as well.
 */
extern void SPLEmitter_EmitParticles(void *self, void *list);

void SPLEmitter_Emit(void *self, void *list) {
    SPLEmitter_EmitParticles(self, list);
}
