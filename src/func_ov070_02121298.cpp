//cpp
extern "C" {
extern char* _ZN5Actor22ClosestNonVanishPlayerEv();
extern int Vec3_Dist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(short* p, short target, short step);
}

extern "C" void func_ov070_02121298(char* c){
    char* p = _ZN5Actor22ClosestNonVanishPlayerEv();
    if (p == 0) {
        *(int*)(c + 0x3a4) = 0;
        return;
    }
    if (Vec3_Dist(c + 0x5c, p + 0x5c) >= 0x2bc000) {
        *(int*)(c + 0x3a4) = 0;
        return;
    }
    *(char**)(c + 0x3a4) = p;
    _Z14ApproachLinearRsss((short*)(c + 0x8e), Vec3_HorzAngle(c + 0x5c, p + 0x5c), 0x800);
}
