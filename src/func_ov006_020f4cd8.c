#include "types.h"
extern int data_0209d4b8;
int RandomIntInternal(int *seed);

void _ZN13dScMgMemory_c12ShuffleCardsEv(char *c)
{
    int slot;
    int k;
    u8 *q;
    int i;
    char *p;

    for (i = 0; i < 7; i++)
        *(u8 *)(c + i + 0x5330) = 0;

    if (*(u8 *)(c + 0x533c) == 0) {
        p = c + 0x60;
        for (k = 4; k < 12; k++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c + 0x60;
        for (i = 4; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    } else if (*(u8 *)(c + 0x533c) == 1) {
        p = c + 0x30;
        for (i = 2; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c + 0x30;
        for (i = 2; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    } else {
        p = c;
        for (i = 0; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c;
        for (i = 0; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    }
}
