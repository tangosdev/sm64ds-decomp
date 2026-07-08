extern int RandomIntInternal(int* seed);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* thiz);
extern int RNG_STATE;

void func_ov060_021169b0(char* thiz) {
    *(int*)(thiz + 0xa8) = 0x1e000;
    *(int*)(thiz + 0x98) = 0xf000;
    *(short*)(thiz + 0x376) = (unsigned int)RandomIntInternal(&RNG_STATE) >> 16;
    _ZN12WithMeshClsn13SetLimMovFlagEv(thiz + 0x110);
    *(int*)(thiz + 0x368) = 0x10;
}
