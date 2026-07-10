/* func_ov006_020c2984 @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b Animation::Finished(). */
extern int _ZN9Animation8FinishedEv(void*);

int func_ov006_020c2984(void* a) {
    return _ZN9Animation8FinishedEv((char*)a + 0x58);
}
