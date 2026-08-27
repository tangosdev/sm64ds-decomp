/* func_ov006_020c0efc @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b Animation::Finished(). */
extern int _ZN9Animation8FinishedEv(void*);

int func_ov006_020c0efc(void* a) {
    return _ZN9Animation8FinishedEv((char*)a + 0x6c);
}
