// @symbol func_ov074_021204c0
/* recovered: Goomboss (daKuriKing_c) shrink/grow interpolation tick.
 *
 * Reached from func_ov074_0211f860 and func_ov074_0211fb44. Picks a pair of
 * float sizes out of data_ov074_02122e4c (12288.0f, 9557.0f, 6826.0f,
 * 4096.0f -- Fix12 scales stored as IEEE singles), divides their difference
 * by the remaining step count in f_607, and steps f_5ec toward the next one.
 * Math_Function_0203b14c eases f_5e4 toward that target; while it is still
 * moving, the scale triple at f_80/f_84/f_88 is driven from a sin/cos pair
 * out of data_02082214 indexed by the f_5fa phase, which advances 0x1230 per
 * frame. Every step below the last also spits a 0xb1 particle a fixed Fix12
 * offset above the boss.
 *
 * The float arithmetic goes through the ITCM soft-float block by hand, the
 * way the rest of this tree calls it: func_01ffa594 (single add/sub),
 * func_01ffa344 (float to int), func_01ffa4bc (int to float) and
 * func_01ff9378, with cstd::fdiv doing the Fix12 divide in between.
 *
 * Shape notes: the particle position temp must stay MEMORY-HOMED -- reading
 * its words back through the address (the ((int *)&v)[n] form) is what keeps
 * the three dead stores the ROM emits; plain member reads scalarize the
 * struct away and cost four instructions. And k has to be computed BEFORE d,
 * which is worth ten words of register identity in the tail: with d first,
 * the index web takes r2 and the f_5e4 web takes r3, exactly reversed from
 * the ROM.
 *
 * Matched byte-for-byte with mwccarm 2004/b56 (ov074); linkcheck VERIFIED.
 */
#include "common.h"

extern int func_01ffa594(int a, int b);
extern int func_01ffa344(int a);
extern int func_01ffa4bc(int a);
extern int func_01ff9378(int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int Math_Function_0203b14c(int* p, int a, int b, int d, int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern int data_ov074_02122e4c[];
extern short data_02082214[];

int func_ov074_021204c0(char* c) {
    struct Vector3 v;
    int d, k;

    *(int*)(c + 0x5ec) = func_01ffa344(func_01ff9378(
        data_ov074_02122e4c[*(unsigned char*)(c + 0x604) + 1],
        func_01ffa4bc(_ZN4cstd4fdivEii(
            func_01ffa344(func_01ffa594(data_ov074_02122e4c[*(unsigned char*)(c + 0x604)],
                                        data_ov074_02122e4c[*(unsigned char*)(c + 0x604) + 1])),
            *(unsigned char*)(c + 0x607) << 12))));

    if (Math_Function_0203b14c((int*)(c + 0x5e4), *(int*)(c + 0x5ec), 0x78, 0x100, 0x20) == 0) {
        if (*(short*)(c + 0x5fa) == 0) {
            *(int*)(c + 0x80) = *(int*)(c + 0x5e4);
            *(int*)(c + 0x84) = *(int*)(c + 0x5e4);
            *(int*)(c + 0x88) = *(int*)(c + 0x5e4);
        }
        if (*(unsigned char*)(c + 0x607) <= 1) return 1;
        (*(unsigned char*)(c + 0x607))--;
        if (*(int*)(c + 0x5cc) == 6) {
            v.x = *(int*)(c + 0x5c);
            v.y = *(int*)(c + 0x60);
            v.z = *(int*)(c + 0x64);
            v.y += (int)(((long long)*(int*)(c + 0x5e4) * (long long)0x190000 + 0x800) >> 12);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb1, ((int*)&v)[0], ((int*)&v)[1], ((int*)&v)[2]);
        }
    }
    k = (*(unsigned short*)(c + 0x5fa) >> 4) * 2;
    d = *(int*)(c + 0x5ec) - *(int*)(c + 0x5e4) + 0x11e;
    *(int*)(c + 0x84) = *(int*)(c + 0x5e4) + (int)(((long long)d * data_02082214[k] + 0x800) >> 12);
    *(int*)(c + 0x80) = *(int*)(c + 0x5e4);
    *(int*)(c + 0x80) = *(int*)(c + 0x5e4) + (int)(((long long)d * data_02082214[k + 1] + 0x800) >> 12);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(short*)(c + 0x5fa) += 0x1230;
    return 0;
}
