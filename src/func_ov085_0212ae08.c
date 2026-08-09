typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern void func_ov085_0212bc78(void *c, void *p);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void _Z14ApproachLinearRsss(s16 *p, s16 t, s16 step);
extern int AngleDiff(int a, int b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *self, void *actor, int b);
extern int func_02013890(int a, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12 d, int e);
extern int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *self, void *ab, u32 id, const Vector3 *pos, u32 e, u32 f);
extern void func_02012694(int a, void *b);
extern int _ZN6Player12GetTalkStateEv(void *p);
extern void _ZN6Player9DropActorEv(void *p);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *pos, const void *rot, int e, int f);
extern void func_02012790(int a);
extern int func_02013944(void);
extern void _ZN7Message7EndTalkEv(void);

extern char data_ov085_021306cc[];
extern char data_ov085_021306bc[];
extern char data_ov085_0213068c[];
extern u8 data_0209d660;
extern u8 data_0209d6bc;

int func_ov085_0212ae08(char *c)
{
    char *pl;
    Vector3 pv;
    Vector3 pos;   /* ShowMessage */
    Vector3 pos7;  /* 43c==7 star spawn */
    Vector3 posR;  /* rabbit star spawn */
    int r4;
    int msg;

    pl = *(char **)(c + 0x45c);
    if (pl == 0) {
        func_ov085_0212bc78(c, data_ov085_021306cc);
        return 1;
    }

    if (*(s32 *)(c + 0x41c) == 0) {
        {
            int *ps = (int *)(((long long)(int)(pl + 0x5c)));
            pv.x = ps[0];
            pv.y = ps[1];
            pv.z = ps[2];
        }

        if (*(u8 *)(c + 0x426) != 0) {
            s16 ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &pv);
            _Z14ApproachLinearRsss((s16 *)(c + 0x94), ang, 0x800);
            if (AngleDiff(*(s16 *)(c + 0x94), ang) > 0x200)
                return 1;
        }

        {
            int guard = (*(s32 *)(c + 0xb0) & 0x4000) ? 1 : 0;
            if (guard == 0) {
                if (*(u8 *)(c + 0x426) != 2)
                    goto after_first_section;
            }
            {
                if (_ZN6Player9StartTalkER9ActorBaseb(pl, c, 1) != 0) {
                    pos.x = *(s32 *)(c + 0x5c);
                    r4 = 0;
                    pos.y = *(s32 *)(c + 0x60);
                    pos.z = *(s32 *)(c + 0x64);

                    if (*(s32 *)(c + 0x43c) == 7)
                        goto msg_13c;
                    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) == 0) {
                        if (*(s32 *)(pl + 8) != 3) {
                            _ZN7Message11PrepareTalkEv();
                            {
                                int z = r4;
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, z);
                            }
                            if (*(s32 *)(c + 0x43c) == 6)
                                goto msg_123a;
                            msg = (s16)(*(s32 *)(pl + 8) + 0x11b);
                            r4 = 0x163;
                            goto have_msg;
                        msg_123a:
                            msg = (s16)(*(s32 *)(pl + 8) + 0x123);
                            r4 = 0x161;
                            goto have_msg;
                        }
                        /* simple msgs for character id 3 */
                        if (*(s32 *)(c + 0x43c) != 6)
                            msg = 0x12b;
                        else
                            msg = 0x12c;
                        goto have_msg;
                    } else {
                        if (*(s32 *)(pl + 8) != 3) {
                            _ZN7Message11PrepareTalkEv();
                            if (*(u8 *)(c + 0x429) == 0) {
                                {
                                    int z = r4;
                                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, z);
                                }
                                if (*(s32 *)(c + 0x43c) == 6)
                                    goto msg_127a;
                                msg = (s16)(*(s32 *)(pl + 8) + 0x11f);
                                r4 = 0x163;
                                goto have_msg;
                            msg_127a:
                                msg = (s16)(*(s32 *)(pl + 8) + 0x127);
                                r4 = 0x161;
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, r4);
                                if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7) {
                                    msg = 0x143;
                                    r4 = 0x160;
                                } else {
                                    r4 = 0x162;
                                    msg = 0x142;
                                }
                            }
                            goto have_msg;
                        }
                        /* character id 3 simple path */
                        if (*(u8 *)(c + 0x429) == 0) {
                            if (*(s32 *)(c + 0x43c) != 6)
                                msg = 0x12d;
                            else
                                msg = 0x12e;
                        } else {
                            if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7)
                                msg = 0x147;
                            else
                                msg = 0x146;
                        }
                    }
                    goto have_msg;
                    msg_13c:
                        msg = 0x13c;
                    have_msg:
                    {
                        int y = pos.y;
                        int zero = 0;
                        y = y + 0x64000;
                        pos.y = y;
                        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(pl, c, (u32)msg, &pos, zero, zero) == 1) {
                            *(s32 *)(c + 0x41c) = 1;
                            if (r4 != 0)
                                func_02012694(r4, c + 0x74);
                        }
                    }
                    return 1;
                }
            }
        }
    }
    after_first_section:
    if (*(volatile s32 *)(c + 0x41c) != 1)
        return 1;

    if (_ZN6Player12GetTalkStateEv(pl) != -1) {
        if (data_0209d660 != 0) {
            if (data_0209d6bc == 9) {
                if (*(s32 *)(c + 0x43c) != 7) {
                    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) != 0) {
                        if (*(u8 *)(c + 0x429) == 0)
                            goto talk_active_done;
                    }
                }
                {
                    int z = 0;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, z, 0x8777, z);
                }
            }
        }
    talk_active_done:
        return 1;
    }

    /* talk ended */
    _ZN6Player9DropActorEv(pl);
    {
        s32 *p128 = (s32 *)(((long long)(int)(c + 0x128)));
        *p128 = *p128 & ~2;
    }
    *(s32 *)(c + 0x98) = 0;

    if (*(s32 *)(c + 0x43c) == 7) {
        pos7.x = *(s32 *)(c + 0x5c);
        pos7.y = *(s32 *)(c + 0x60);
        pos7.z = *(s32 *)(c + 0x64);
        pos7.y = pos7.y + 0x32000;
        {
            s8 cc = *(s8 *)(c + 0xcc);
            int m1 = -1;
            void *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xe5, *(s32 *)(c + 0x43c), &pos7, (void *)(c + 0x8c), cc, m1);
            if (spawned != 0)
                *(s32 *)((char *)spawned + 0x190) = *(s32 *)(c + 4);
        }
        func_02012790(0xa);
        *(u8 *)(c + 0x427) = 0;
        *(s32 *)(c + 0x45c) = 0;
        func_ov085_0212bc78(c, data_ov085_021306bc);
        return 1;
    }

    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) != 0) {
        if (*(u8 *)(c + 0x429) == 0)
            goto no_spawn;
        if (_ZN8SaveData22NumGlowingRabbitsFoundEv() != 7)
            goto no_spawn;
    }
    {
        posR.x = *(s32 *)(c + 0x5c);
        posR.y = *(s32 *)(c + 0x60);
        posR.z = *(s32 *)(c + 0x64);
        {
            u32 param = *(s32 *)(c + 0x43c);
            if (*(u8 *)(c + 0x429) != 0) {
                param = 0x4d;
                func_02013944();
            }
            posR.y = posR.y + 0x32000;
            {
                s8 cc = *(s8 *)(c + 0xcc);
                int m1 = -1;
                void *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xe5, param, &posR, 0, cc, m1);
                if (spawned != 0)
                    *(s32 *)((char *)spawned + 0x190) = *(s32 *)(c + 4);
            }
            func_02012790(0xa);
            *(u8 *)(c + 0x427) = 0;
            {
                u16 *pf = (u16 *)(((long long)(int)(pl + 0x6ce)));
                *pf = (u16)(*pf | 0x800);
            }
        }
        goto after_spawn;
    }
no_spawn:
    if (*(u8 *)(c + 0x427) == 2) {
        {
            u16 *pf = (u16 *)(((long long)(int)(pl + 0x6ce)));
            *pf = (u16)(*pf & ~0x800);
        }
        *(u8 *)(c + 0x427) = 0;
    }
    if (*(u8 *)(c + 0x429) == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0);
        _ZN7Message7EndTalkEv();
    }
after_spawn:
    ;

    *(s32 *)(c + 0x45c) = 0;

    if (*(u8 *)(c + 0x429) == 0)
        goto do_306bc;
    if (_ZN8SaveData22NumGlowingRabbitsFoundEv() != 8)
        goto flag_path;
do_306bc:
    func_ov085_0212bc78(c, data_ov085_021306bc);
    goto final_return;
flag_path:
    {
        u16 *pf = (u16 *)(((long long)(int)(pl + 0x6ce)));
        *pf = (u16)(*pf | 0x800);
    }
    *(char **)(c + 0x460) = pl;
    func_ov085_0212bc78(c, data_ov085_0213068c);
final_return:
    return 1;
}
