// @symbol func_ov098_0213b7e8
// @emits Cannon_Kill
/* recovered: renamed to Class_Method */
/* daCnn_c::Kill - recovered from vtable slot identity */
typedef unsigned char u8;
typedef unsigned short u16;

extern int _Z14ApproachLinearRiii(int *v, int step, int rate);
extern void func_ov098_0213b63c(char *c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern char *_ZN5Actor13ClosestPlayerEv(char *self);
extern short Vec3_HorzAngle(const void *v0, const void *v1);

void Cannon_Kill(char *c)
{
    int state = *(int *)(c + 0x3c0);
    if (state == 0) {
        if (*(u8 *)(c + 0x3b6) >= 3) {
            if (_Z14ApproachLinearRiii((int *)(c + 0x3bc), 0x100, *(int *)(c + 0x3b8)) != 0) {
                func_ov098_0213b63c(c);
                return;
            }
        } else {
            if (_Z14ApproachLinearRiii((int *)(c + 0x3bc), 0x300, *(int *)(c + 0x3b8)) != 0)
                *(int *)(((int)c + 0x3c0) & 0xFFFFFFFFFFFFFFFF) += 1;
        }
        *(int *)(c + 0x3b8) = _ZN4cstd4fdivEii((int)(((long long)*(int *)(c + 0x3b8) * 0xb00 + 0x800) >> 12), 0x1000);
        *(int *)(c + 0x84) = *(int *)(c + 0x3bc);
        *(int *)(c + 0x88) = 0x2000 - *(int *)(c + 0x84);
        *(int *)(c + 0x80) = *(int *)(c + 0x88);
    } else if (state == 1) {
        int approach = _Z14ApproachLinearRiii((int *)(c + 0x3bc), 0x1000, *(int *)(c + 0x3b8));
        if (approach != 0) {
            char *player;
            int *pst = (int *)(((int)c + 0x3c0) & 0xFFFFFFFFFFFFFFFF);
            *pst = *pst + 1;
            *(int *)(c + 0xa8) = (*(u8 *)(c + 0x3b6) << 13) + 0x30000;
            *(int *)(c + 0x98) = 0x6000;
            player = _ZN5Actor13ClosestPlayerEv(c);
            if (player != 0) {
                *(u16 *)(c + 0x94) = Vec3_HorzAngle(c + 0x5c, player + 0x5c);
            } else {
                func_ov098_0213b63c(c);
                return;
            }
        }
        *(int *)(c + 0x3b8) = _ZN4cstd4fdivEii((int)(((long long)*(int *)(c + 0x3b8) * 0x1400 + 0x800) >> 12), 0x1000);
        *(int *)(c + 0x84) = *(int *)(c + 0x3bc);
        *(int *)(c + 0x88) = 0x2000 - *(int *)(c + 0x84);
        *(int *)(c + 0x80) = *(int *)(c + 0x88);
    } else {
        if (*(int *)(c + 0xa8) >= 0) {
            *(int *)(c + 0x80) = 0xf00;
            *(int *)(c + 0x84) = 0x1200;
            *(int *)(c + 0x88) = 0xf00;
        } else {
            *(int *)(c + 0x3c4) = 1;
            *(int *)(c + 0x80) = 0x1000;
            *(int *)(c + 0x84) = 0x1000;
            *(int *)(c + 0x88) = 0x1000;
        }
    }
}
