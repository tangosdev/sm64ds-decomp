typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dActor_c dActor_c;

extern int data_0209f32c;
extern char data_ov002_0211067c;

extern int _ZN6Player7IsStateERNS_5StateE(void* self, void* state);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(const void* self);
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, const Vector3* a, const Vector3* b, dActor_c* obj);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* res, void* self);

/* the -= 0x4000 goes through a materialized base (add r1,c,#0x60; ldr/str [r1]);
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x60]. */
#define LDR_I(p) (*(int*)(int)(p))

void func_ov002_020ce798(char* c)
{
    char rl[0x78];
    Vector3 v1, v2;
    Vector3 cp;

    if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211067c)) return;

    if (*(int*)(c + 0x60) >= data_0209f32c - 0x50000) {
        if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
            *(int*)(c + 0x60) = data_0209f32c - 0x50000;
    }

    _ZN9dBgCh_LinC1Ev(rl);
    v1.x = *(int*)(c + 0x5c);
    v1.y = *(int*)(c + 0x60);
    v1.z = *(int*)(c + 0x64);
    v2.x = *(int*)(c + 0x5c);
    v2.y = *(int*)(c + 0x60);
    v2.z = *(int*)(c + 0x64);
    v1.y += 0x50000;
    v2.y += 0x96000;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &v1, &v2, (dActor_c*)c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl)) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&cp, rl);
        if (*(int*)(c + 0x60) >= cp.y - 0x82000) {
            LDR_I(c + 0x60) -= 0x4000;
            if (*(int*)(c + 0x60) < cp.y - 0x82000)
                *(int*)(c + 0x60) = cp.y - 0x82000;
        }
    }
    _ZN9dBgCh_LinD1Ev(rl);
}
