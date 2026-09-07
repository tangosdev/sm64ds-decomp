typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct { s32 x, y, z; } Vector3;
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int i, int fix, u32 j);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *self);
extern void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *bmd, void *bma);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *self, void *file, int i, int fix, u32 j);
extern void func_ov084_021296cc(char *c);
extern void _ZN12dEnemyBase_c9SpawnCoinEv(void *self);
extern void func_ov084_02129498(char *r0);
extern void _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(void *self, const Vector3 *v);
extern void _ZN11dCapEnemy_c15RespawnIfHasCapEv(void *self);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_02012694(int a, void *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(void *self, void *c);
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov002_020aea30(void *a, void *b, void *c, int d);
extern void *data_ov084_02130ce8[];
extern void *data_ov084_02130228[];
extern void *data_ov084_02130cf8[];
extern void *data_ov084_0213089c;
extern void *data_ov084_02130ce0[];
void func_ov084_0212a774(char *c)
{
    Vector3 v;
    u16 h = *(u16 *)(c + 0x400 + 0x52);

    if (h == 0) {
        _ZN9Animation7AdvanceEv(c + 0x3fc);
        if (_ZN9Animation8FinishedEv(c + 0x3c0) == 0)
            return;
        *(s32 *)(c + 0xb0) = *(s32 *)(c + 0x44c);
        *(s32 *)(c + 0x434) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
        *(void **)(c + 0x444) = data_ov084_02130228[*(s32 *)(c + 0x460)];
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1b4);
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            *f198 = *f198 & ~0x20000;
        }
        _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(data_ov084_02130cf8[1], &data_ov084_0213089c);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);
        *(s32 *)(c + 0x404) = 0;
        return;
    }
    if (h <= 0x3c) {
        *(u16 *)(((long long)(int)(c + 0x452))) -= 1;
        if (*(u16 *)(c + 0x400 + 0x52) == 0) {
            func_ov084_021296cc(c);
            _ZN12dEnemyBase_c9SpawnCoinEv(c);
            func_ov084_02129498(c);
            v.x = 0;
            v.y = 0x6c000;
            v.z = 0;
            _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(c, &v);
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x41c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x420);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x424);
            _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
        }
    }
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1b4) != 0) {
        int a8;
        int cnt;
        if (*(u16 *)(c + 0x452) > 0x3c)
            *(u16 *)(c + 0x452) = 0x1e;
        a8 = *(s32 *)(c + 0xa8);
        cnt = *(u16 *)(c + 0x452);
        {
            int aa = a8 < 0 ? -a8 : a8;
            if (aa <= cnt * 0x500) {
                *(s32 *)(c + 0xa8) = (cnt << 0xa) - *(s32 *)(c + 0x9c);
            } else {
                *(s32 *)(c + 0xa8) = _ZN4cstd4fdivEii((a8 * -0x50) / 100, *(s32 *)(c + 0xd8));
            }
        }
        {
            s32 *p98 = (s32 *)(((long long)(int)(c + 0x98)));
            *p98 >>= 1;
        }
        {
            int v98 = *(s32 *)(c + 0x98);
            int av = v98 < 0 ? -v98 : v98;
            if (av < 0x5000) {
                if (v98 < 0)
                    *(s32 *)(c + 0x98) = -0x5000;
                else
                    *(s32 *)(c + 0x98) = 0x5000;
            }
        }

        if (*(u8 *)(c + 0x467) == 0) {
            func_02012694(0x13a, c + 0x74);
        } else if (*(u8 *)(c + 0x467) <= 2) {
            func_02012694(0x13b, c + 0x74);
        }
        *(u8 *)(((long long)(int)(c + 0x467))) += 1;
    } else {
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1b4) != 0) {
            _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1b4);
            *(s32 *)(c + 0x98) = 0;
            *(s32 *)(c + 0xa8) = 0;
            if (*(u16 *)(c + 0x452) > 0x3c)
                *(u16 *)(c + 0x452) = 0x1e;
        }
    }
    *(u8 *)(c + 0x107) = 1;
    if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(c, c + 0x180) != 0) {
        void *a = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x1a4));
        *(s32 *)(c + 0x10c) = 7;
        func_ov002_020aea30(c, a, c + 0x1b4, 7);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *f198 |= 1;
        }
    }
    *(u8 *)(c + 0x107) = 0;
}
