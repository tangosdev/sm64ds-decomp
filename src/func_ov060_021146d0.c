#include "types.h"
/* func_ov060_021146d0 at 0x021146d0 (ov060), size 0x188
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern void func_02012694(int a, void *b);
extern void func_ov060_02111cc0(char *c, int idx, int m);
extern void func_ov060_02115a84(char *c, char *arg);
extern int Bowser_IsAnimAtLastFrame(char *c);

void func_ov060_021146d0(char *c) {
    if (*(u16*)(c + 0x300 + 0xfc) == 0) {
        *(int*)(c + 0x98) = -0x1c000;
        *(int*)(c + 0xa8) = 0x50000;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x400 + 8) + 0x8000;
        *(u8*)(c + 0x422) = 1;
        *(s16*)(c + 0x8c) = -0xc00;
        func_02012694(0xb1, c + 0x74);
    } else {
        if (*(u16*)(c + 0x8c) > 0xc00) {
            s16 *p8c = (s16*)(((long long)(int)((char*)c + 0x8c)));
            *p8c = *p8c - 0xc00;
        } else {
            *(u16*)(c + 0x8c) = 0;
        }
    }
    {
        u8 st = *(u8*)(c + 0x423);
        if (st == 0) {
            func_ov060_02111cc0(c, 1, 0x40000000);
            {
                u8 *p = (u8*)(((long long)(int)((char*)c + 0x423)));
                *p = *p + 1;
            }
            *(u16*)(c + 0x300 + 0xfe) = 0;
            return;
        }
        if (st == 1) {
            func_ov060_02115a84(c, c + 0x3fe);
            if (*(u16*)(c + 0x300 + 0xfe) == 1)
                func_ov060_02111cc0(c, 2, 0x40000000);
            if (*(u16*)(c + 0x300 + 0xfe) < 3) return;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0;
            {
                u8 *p = (u8*)(((long long)(int)((char*)c + 0x423)));
                *p = *p + 1;
            }
            return;
        }
        if (st != 2) return;
        if (Bowser_IsAnimAtLastFrame(c) != 0) {
            if (*(s8*)(c + 0x400 + 0x1e) == 1) *(int*)(c + 0x40c) = 3;
            else *(int*)(c + 0x40c) = 0;
        }
        *(u8*)(c + 0x422) = 0;
    }
}