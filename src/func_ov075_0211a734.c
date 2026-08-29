// @symbol func_ov075_0211a734
// recovered name: dScEntry_c_BeforeInitResources
/* recovered: renamed to Class_Method */
/* dScEntry_c::BeforeInitResources - byte-matched vs the ROM at mwccarm 2004/b56, strict relocs
   (tools/match.py, size from ov075 symbols.txt). The old vtable-slot-guess
   marker was wrong: the body reproduces the ROM's bytes. VS wiring lane. */
/* func_ov075_0211a734 @ 0x211a734 (ov075) -- tail-call veneer to _ZN5Scene19ResetFadersAndSoundEv (0x202e66c).
 * ldr ip, [pc]; bx ip; .word 0x202e66c
 */
extern void _ZN5Scene19ResetFadersAndSoundEv(void);

void func_ov075_0211a734(void) {
    _ZN5Scene19ResetFadersAndSoundEv();
}
