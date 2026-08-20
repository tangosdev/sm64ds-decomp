typedef struct { int x, y, z; } Vec3;

extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void*);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void*);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern void Scuttlebug_SetState(void*, int);
extern void func_ov071_0211f29c(void*);
extern void _ZN5dCc_c5ClearEv(void*);
extern void _ZN5dCc_c6UpdateEv(void*);

int func_ov071_02120028(char *c)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x160);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x194)) {
        Vec3 v;
        int x, y, z;
        x = *(volatile int*)(c + 0x5c);
        *(volatile int*)&v.x = x;
        y = *(volatile int*)(c + 0x60);
        *(volatile int*)&v.y = y;
        z = *(volatile int*)(c + 0x64);
        y += 0x28000;
        *(volatile int*)&v.z = z;
        *(volatile int*)&v.y = y;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, x, y, z);
        *(int*)(c + 0xa8) = *(int*)(c + 0xa8) * -0x28 / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194)) {
        *(int*)(c + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x194);
        *(int*)(c + 0x390) = *(int*)(c + 0x5c);
        *(int*)(c + 0x394) = *(int*)(c + 0x60);
        *(int*)(c + 0x398) = *(int*)(c + 0x64);
        *(int*)(c + 0xb0) |= 0x10000001;
        Scuttlebug_SetState(c, 2);
    }
    func_ov071_0211f29c(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    return 1;
}
