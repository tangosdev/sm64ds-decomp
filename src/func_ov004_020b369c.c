/* func_ov004_020b369c at 0x020b369c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern s16 data_02082214[];
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);

void func_ov004_020b369c(char* self) {
    char* el = self + 0x34;
    int angle = (u16)*(int*)(self + 0x24);
    struct M m;
    u16 flag;
    do {
        int s0 = data_02082214[(angle >> 4) * 2];
        int mm = (int)(((s64)s0 * 0x800 + 0x800) >> 12);
        int idx = (u16)mm >> 4;
        int cos1 = data_02082214[idx * 2 + 1];
        int sin1 = data_02082214[idx * 2];
        int fc = (int)(((s64)cos1 * 0x1000 + 0x800) >> 12);
        int fs = (int)(((s64)sin1 * 0x1000 + 0x800) >> 12);
        m._00 = fc;
        m._11 = fc;
        m._01 = fs;
        m._10 = -fs;
        func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                            *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        angle = (u16)(angle + 0x4000);
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != 0xffff);
}
