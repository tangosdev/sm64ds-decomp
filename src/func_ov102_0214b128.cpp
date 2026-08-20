//cpp
extern "C" {
extern int _ZN8dActor_c7FindEggER5dCc_c(void*, void*);
extern int func_02010304(void* target, char* p);
extern void func_ov102_0214b3f0(void* c, int a1);
extern void func_ov102_0214b384(void* c, int a1);
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
extern void func_ov102_0214bc20(void* c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void*);
void func_ov102_0214b128(char* c) {
    int t;
    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x110) || (*(int*)(c + 0x130) & 0x20000)) {
        int* p = (int*)(((int)c + 0xb0));
        *(short*)(c + 0x3ea) = 4;
        *p = *p & ~1;
        *(int*)(c + 0x98) = 0;
        return;
    }
    t = func_02010304(c, c + 0x110);
    if (t) {
        func_ov102_0214b3f0(c, t);
        return;
    }
    t = *(int*)(c + 0x130);
    if (t & 0x40000) {
        func_ov102_0214b384(c, 4);
        return;
    }
    if (!(t & 0x380)) return;
    t = *(int*)(c + 0x3dc);
    if (t < 0) return;
    if (t > 2) return;
    {
        void* r5 = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x134));
        if (!r5) return;
        func_ov102_0214bc20(c);
        *(short*)(c + 0x94) = *(short*)((char*)r5 + 0x8e);
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x110);
        _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x144);
    }
}
}
