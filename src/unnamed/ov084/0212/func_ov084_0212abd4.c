#include "types.h"
extern void func_ov084_02129cf4(void *self, int a);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern void func_ov084_02129c9c(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int i, int fx, unsigned int j);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern int _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(void *self, void *clsn, short *a);
extern int RandomIntInternal(int *seed);
extern int data_ov084_02130228[];
extern int data_ov084_02130268[];
extern void *data_ov084_02130cf0[];
extern void *data_ov084_02130ce8[];
extern int data_0209e650;
typedef struct { unsigned char b0 : 1; } Flag;
void func_ov084_0212abd4(char *self)
{
    short step = 0x200;
    func_ov084_02129cf4(self, 0x3e8000);
    _Z14ApproachLinearRiii((int *)(self + 0x98), *(int *)(self + 0x444), 0x500);
    if (((Flag *)(self + 0x468))->b0) {
        if (_Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45a), step)) {
            *(unsigned char *)(((int)self + 0x468)) &= ~1;
            return;
        }
        {
            unsigned char *p = (unsigned char *)(((int)self + 0x468));
            *p = (*p & ~1) | 1;
        }
        return;
    }
    if (*(u16 *)(self + 0x458) != 0) {
        if (*(unsigned char *)(self + 0x113) < 6) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            step = 0x800;
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
            *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x45c) = Vec3_HorzAngle(self + 0x5c, self + 0x41c);
            step = 0x400;
        }
        _Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
        return;
    }
    if (*(int *)(self + 0x440) >= 0x61a8000) {
        *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        *(s16 *)(self + 0x450) = 0x19;
    }
    {
        int bit = _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(
                      self, self + 0x1b4, (short *)(self + 0x45c));
        unsigned char *p = (unsigned char *)(((int)self + 0x468));
        bit &= 1;
        *p = (*p & ~1) | bit;
    }
    if (!((Flag *)(self + 0x468))->b0) {
        if (*(int *)(self + 0x440) < *(int *)(self + 0x448) ||
            (*(unsigned char *)(self + 0x113) < 6 &&
             Vec3_Dist(self + 0x5c, self + 0x41c) > 0x3e8000)) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            if (*(unsigned char *)(self + 0x113) >= 6 || *(u16 *)(self + 0x458) != 0) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            } else {
                step = 0x600;
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            }
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            
            if (*(u16 *)(self + 0x450) != 0) {
                *(u16 *)(((int)self + 0x450)) =
                    *(u16 *)(((int)self + 0x450)) - 1;
            } else if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 3) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x94) + (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                *(u16 *)(self + 0x450) = 0x64;
            } else {
                *(s16 *)(self + 0x45c) = (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                func_ov084_02129c9c(self);
            }

        }
    }
    if (*(unsigned char *)(self + 0x113) >= 6) {
        if (*(u16 *)(self + 0x456) > 0x1e)
            *(u16 *)(self + 0x458) = *(u16 *)(self + 0x456);
    }
    _Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
}
