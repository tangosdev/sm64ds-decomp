//cpp
typedef int Fix12i;
extern "C" void func_ov002_020bfa74(void* self);
extern "C" void func_ov002_020c0108(void* self, int x);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
extern "C" void func_ov002_020ce798(void* self);

extern "C" void func_ov002_020bf36c(char* self, void* arg){
    Fix12i saved;
    unsigned char f = *(unsigned char*)(self + 0x709);
    if (f == 0)
        func_ov002_020bfa74(self);
    if ((*(unsigned char*)(self + 0x6e9) & 1) || *(unsigned char*)(self + 0x706))
        func_ov002_020c0108(self, 1);
    if (*(int*)(self + 0x37c) != 0)
        return;
    saved = *(Fix12i*)(self + 0x98);
    if (*(unsigned char*)(self + 0x6e9) & 1)
        *(Fix12i*)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i*)(self + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i*)(self + 0x98) = saved;
    func_ov002_020ce798(self);
}
