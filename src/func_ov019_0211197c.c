typedef unsigned char u8;
typedef unsigned int u32;

typedef struct { int x, y, z; } Vector3;

#define LB(off) (*(u8 *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFF))

extern int func_ov019_0211131c(void *self);
extern void func_ov019_021113b0(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int frame, int rate, u32 flags);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern int func_ov019_0211140c(void *self, void *clsn);
extern int func_ov019_0211127c(void *self, Vector3 *v, u32 j);
extern int _ZNK7PathPtr8NumNodesEv(void *self);
extern int func_ov019_02111254(void *self, int d);
extern void _Z14ApproachLinearRiii(int *p, int a, int b);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 cc, void *v, u32 d);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int cc, int d, int e, void *f, void *g);
extern void func_ov019_021122dc(void *self, int a);
extern void func_ov019_021112b8(void *self);

extern void *data_ov019_02113488[];
extern void *data_ov019_02113490[];
extern Vector3 data_ov019_021134e8;
extern Vector3 data_ov019_0211353c;

int func_ov019_0211197c(void *self)
{
    u8 *c = (u8 *)self;

    switch (c[0x38f]) {
    case 0:
        if (func_ov019_0211131c(c)) {
            func_ov019_021113b0(c);
        }
        if (_ZN9Animation8FinishedEv(c + 0x124)) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov019_02113488[1], 0, 0x1000, 0);
            *(int *)(c + 0x130) = 0x1000;
            LB(0x38f)++;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x174);
        func_ov019_0211140c(c, c + 0x1a8);
        if (c[0x394] == 0 && c[0x393] == 0) {
            {
                Vector3 v;
                v.x = *(int *)(c + 0x5c);
                v.y = *(int *)(c + 0x60);
                v.z = *(int *)(c + 0x64);
                if (func_ov019_0211127c(c, &v, c[0x390])) {
                    LB(0x390)++;
                }
            }
            {
                u8 *o = *(u8 **)(c + 0x378);
                u8 *p = (u8 *)(((int)o + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (func_ov019_0211127c(c, &v, c[0x391])) {
                    LB(0x391)++;
                }
            }
        }
        break;

    case 1:
        if (func_ov019_0211131c(c)) {
            if (*(int *)(c + 0x36c) >= _ZNK7PathPtr8NumNodesEv(c + 0x364) - 2) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov019_02113490[1], 0x40000000, 0x1000, 0);
                LB(0x38f)++;
            } else {
                func_ov019_021113b0(c);
            }
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x174);
        func_ov019_0211140c(c, c + 0x1a8);
        if (c[0x394] == 0 && c[0x393] == 0 &&
            *(int *)(c + 0x36c) < _ZNK7PathPtr8NumNodesEv(c + 0x364) - 2) {
            {
                Vector3 v;
                v.x = *(int *)(c + 0x5c);
                v.y = *(int *)(c + 0x60);
                v.z = *(int *)(c + 0x64);
                if (func_ov019_0211127c(c, &v, c[0x390])) {
                    LB(0x390)++;
                }
            }
            {
                u8 *o = *(u8 **)(c + 0x378);
                u8 *p = (u8 *)(((int)o + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (func_ov019_0211127c(c, &v, c[0x391])) {
                    LB(0x391)++;
                }
            }
        }
        if (c[0x391] > 1) {
            int val = func_ov019_02111254(c, c[0x391] - c[0x390]);
            if (c[0x394] != 0 || c[0x393] != 0) {
                val = 0x65000;
            }
            _Z14ApproachLinearRiii((int *)(c + 0x98), val, 0x800);
        }
        if (c[0x392] == 0 && Vec3_Dist(&data_ov019_021134e8, (Vector3 *)(c + 0x5c)) < 0x190000) {
            if (c[0x393] != 0) {
                *(u8 *)(c + 0x395) = 0;
            } else {
                *(u8 *)(c + 0x395) = 1;
            }
            *(u8 *)(c + 0x392) = 1;
        }
        *(u32 *)(c + 0x384) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(u32 *)(c + 0x384), 3, 0x185, c + 0x74, 0);
        *(void **)(c + 0x388) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x388), 0x101, *(int *)(c + 0x5c), *(int *)(c + 0x60) + 0x32000, *(int *)(c + 0x64), 0, 0);
        break;

    case 2:
        if (_ZN9Animation8FinishedEv(c + 0x124)) {
            func_ov019_021122dc(c, 4);
        }
        break;
    }

    func_ov019_021112b8(c);
    if (Vec3_Dist(&data_ov019_0211353c, (Vector3 *)(*(u8 **)(c + 0x378) + 0x5c)) < 0x280000) {
        *(u8 *)(c + 0x394) = 1;
    }
    if (Vec3_Dist(&data_ov019_021134e8, (Vector3 *)(*(u8 **)(c + 0x378) + 0x5c)) < 0x190000) {
        *(u8 *)(c + 0x393) = 1;
    }
    return 1;
}
