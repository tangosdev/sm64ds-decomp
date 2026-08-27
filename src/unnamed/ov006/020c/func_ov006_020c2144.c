/* func_ov006_020c2144 @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b BlendModelAnim::Advance(). */
extern void _ZN14BlendModelAnim7AdvanceEv(void*);

void func_ov006_020c2144(void* a) {
    _ZN14BlendModelAnim7AdvanceEv((char*)a + 0x10);
}
