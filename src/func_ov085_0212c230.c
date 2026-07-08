typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern s8 LEVEL_ID;
extern s8 AREA_ID;
extern int SAVE_DATA[];
extern void* CASTLE_KEY_RABBIT;
extern char data_ov085_021305c0;
extern char data_ov085_0213068c;
extern char data_ov085_021306ac;
extern char data_ov085_021306bc;
extern char data_ov085_021306dc;

extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, void* f, void* g);
extern void _ZN7Message11PrepareTalkEv(void);
extern int func_02013890(int a, int b);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12 d, int e);
extern int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* thiz, void* ab, u32 id, const Vector3* pos, u32 e, u32 f);
extern void func_02012694(int a, void* b);
extern int _ZN6Player12GetTalkStateEv(void* c);
extern void _ZN6Player9DropActorEv(void* c);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* c, void* w);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void func_ov085_0212bcc8(void* c);
extern void func_ov085_0212bc78();
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* c, void* f, int i, Fix12 fx, u32 j);
extern u16 DecIfAbove0_Short(void* p);
extern void _ZN9Animation7AdvanceEv(void* c);
extern void func_ov085_0212bdbc(void* c);
extern void func_ov085_0212bedc(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* cyl);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* c, void* w, u32 j);
extern void func_ov085_0212a828(void* c);

int func_ov085_0212c230(void* arg0)
{
    char* c = (char*)arg0;
    void* r0p;

    if (LEVEL_ID != 0x32) {
        if (*(u8*)(c + 0x428) == 1) {
            if (SAVE_DATA[2] & 0x20000)
                *(u8*)(c + 0x428) = 0;
            return 1;
        }
    } else {
        r0p = _ZN5Actor13ClosestPlayerEv(c);
        if (r0p == 0)
            return 1;
        if (!(SAVE_DATA[2] & 0x20000) || *(s32*)(c + 0x440) != *(u8*)((char*)r0p + 0x6d9)) {
            *(u8*)(c + 0x428) = 1;
            return 1;
        }
        *(u8*)(c + 0x428) = 0;
    }

    if (*(s32*)(c + 0x43c) == 7 && !(SAVE_DATA[1] & 0x40))
        CASTLE_KEY_RABBIT = c;

    if (*(u8*)(c + 0x429) != 0) {
        volatile Vector3 pv;
        pv.x = *(s32*)(c + 0x5c);
        pv.y = *(s32*)(c + 0x60);
        pv.z = *(s32*)(c + 0x64);
        pv.y = *(s32*)(c + 0x60) + 0x3c000;
        *(void**)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile u32*)(c + 0x470), 0x10d, pv.x, pv.y, *(s32*)(c + 0x64), 0, 0);
    }

    {
        u32 temp_r1 = *(u8*)(c + 0x427);
        if (temp_r1 < 2) {
            int v = (*(s32*)(c + 0xb0) & 0x40000) ? 1 : 0;
            if (v == 1) {
                void* temp_r4 = *(void**)(c + 0x45c);
                if (temp_r4 != 0) {
                    if (temp_r1 == 0) {
                        volatile Vector3 pos;
                        int var_r6;
                        int var_r2;
                        int t;
                        pos.x = *(s32*)(c + 0x5c);
                        pos.y = *(s32*)(c + 0x60);
                        pos.z = *(s32*)(c + 0x64);
                        _ZN7Message11PrepareTalkEv();
                        t = *(s32*)(c + 0x43c);
                        if (t != 7) {
                            if (func_02013890(t, *(s32*)((char*)temp_r4 + 8)) == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (*(s32*)(c + 0x43c) != 6) { var_r6 = 0x162; var_r2 = 0x11e; }
                                else { var_r6 = 0x160; var_r2 = 0x126; }
                            } else if (*(u8*)(c + 0x429) == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0);
                                if (*(s32*)(c + 0x43c) != 6) { var_r6 = 0x162; var_r2 = 0x122; }
                                else { var_r6 = 0x160; var_r2 = 0x12a; }
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7) { var_r2 = 0x145; var_r6 = 0x160; }
                                else { var_r6 = 0x162; var_r2 = 0x144; }
                            }
                        } else {
                            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                            var_r2 = 0x13b; var_r6 = 0x160;
                        }
                        pos.y += 0x64000;
                        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(temp_r4, c, var_r2, 0, 0, 0) == 1) {
                            func_02012694(var_r6, c + 0x74);
                            *(u8*)(c + 0x427) = 1;
                        }
                    } else if (temp_r1 == 1 && _ZN6Player12GetTalkStateEv(temp_r4) == -1) {
                        _ZN6Player9DropActorEv(temp_r4);
                        *(u8*)(c + 0x427) = 2;
                        *(u16*)(((long long)(int)((char*)temp_r4 + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x800;
                    }
                }
            }
        }
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x144) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(u8*)(c + 0x107) != 0) {
            if (*(u16*)(c + 0x104) == 5)
                *(s32*)(c + 0x98) = 0;
            if (*(u16*)(c + 0x104) == 0)
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
        if (*(u8*)(c + 0x107) == 1) {
            *(u8*)(((long long)(int)(c + 0x42a)) & 0xFFFFFFFFFFFFFFFFLL) = *(u8*)(((long long)(int)(c + 0x42a)) & 0xFFFFFFFFFFFFFFFFLL) + 1;
            if (*(u8*)(c + 0x42a) > 0x96) {
                *(u8*)(c + 0x107) = 0;
                *(u8*)(c + 0x42a) = 0;
            }
        }
        func_ov085_0212bcc8(c);
        *(u8*)(c + 0x426) = 2;
        func_ov085_0212bc78(c, &data_ov085_021306ac);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        if (*(void**)(c + 0x45c) == 0)
            *(void**)(c + 0x45c) = _ZN5Actor13ClosestPlayerEv(c);
        return 1;
    }

    if (*(u8*)(c + 0x426) != 0) {
        void* p = *(void**)(c + 0x364);
        if (p != (void*)&data_ov085_021306ac && p != (void*)&data_ov085_0213068c &&
            p != (void*)&data_ov085_021306bc && p != (void*)&data_ov085_021306dc) {
            func_ov085_0212bc78(c, &data_ov085_021306ac);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        }
        *(s32*)(c + 0x98) = 0;
    }

    DecIfAbove0_Short(c + 0x100);

    {
        int* p = *(int**)(c + 0x364);
        if (p[2] != 0) {
            int* q = p + 2;
            int adj = q[1];
            void* thiz = c + (adj >> 1);
            void (*fn)(void*);
            if (adj & 1)
                fn = *(void(**)(void*))(*(char**)thiz + q[0]);
            else
                fn = (void(*)(void*))q[0];
            fn(thiz);
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x350);

    {
        int v = (*(s32*)(c + 0xb0) & 0x4000) ? 1 : 0;
        if (v != 0)
            func_ov085_0212bdbc(c);
        else
            func_ov085_0212bcc8(c);
    }

    if (LEVEL_ID == 5 && AREA_ID == 3)
        func_ov085_0212bedc(c);

    {
        int v = (*(s32*)(c + 0xb0) & 0x4000) ? 1 : 0;
        if (v == 0) {
            if (*(void**)(c + 0x364) != (void*)&data_ov085_021306ac || *(u8*)(c + 0x426) != 0)
                _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
            *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
            if (*(void**)(c + 0x364) != (void*)&data_ov085_021306ac)
                func_ov085_0212a828(c);
        }
    }

    if (*(void**)(c + 0x364) != (void*)&data_ov085_021306ac) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }

    return 1;
}
