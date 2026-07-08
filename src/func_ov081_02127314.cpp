//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int RandomIntInternal(void* s);
extern int data_ov081_02128ecc[];
extern int RNG_STATE[];
int func_ov081_02127314(char* c){
    int *base = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFULL);
    *base = *base & ~1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov081_02128ecc[1], 0x40000000, 0x1000, 0);
    *(int*)(c+0x12c) = 0;
    *(short*)(c+0x8e) = (short)RandomIntInternal(RNG_STATE);
    *(short*)(c+0x94) = *(short*)(c+0x8e);
    *(int*)(c+0x98) = 0;
    *(unsigned char*)(c+0x3f1) = 0;
    *(int*)(c+0x3e0) = 3;
    return 1;
}
}
