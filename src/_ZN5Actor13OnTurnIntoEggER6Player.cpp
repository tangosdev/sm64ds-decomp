//cpp
/* _ZN5Actor13OnTurnIntoEggER6Player @ 0x2010154 (arm9) -- tail-call veneer to _ZN5Actor24KillAndTrackInDeathTableEv (0x200f9b8).
 * ldr ip, [pc]; bx ip; .word 0x200f9b8
 */
extern "C" {
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void);
void _ZN5Actor13OnTurnIntoEggER6Player(void) {
    _ZN5Actor24KillAndTrackInDeathTableEv();
}
}
