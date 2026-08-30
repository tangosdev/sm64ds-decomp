// @symbol func_ov075_0211afb0
/* recovered: dScEntry_c draws the VS/wireless entry-menu grid mesh.
 *
 * Concatenates the object's own matrix (self+0x14) with the pooled camera
 * matrix at data_0209b3ec and pushes the result as both the position and the
 * directional matrix, sets a full-white diffuse/ambient and specular/emission
 * pair, then hands the material state to func_ov075_0211a948. The mesh is a
 * (self[0xa6] - 1) x self[0xa7] grid of triangle strips over the 0x18-byte
 * vertex array at self+0x80: each inner iteration emits vertex (i, j) and
 * vertex (i + 1, j), converting each 1.19.12 coordinate to the VTX_16 format
 * with (v << 9) >> 16 and packing x and y into one command word.
 */
// NONMATCHING: register allocation (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 2004/b56. The residual is four words in the
// SECOND vertex emission (+0x164..+0x170): the ROM routes x<<9 through the dying
// byte-offset register and z<<9 through the freed array base, where mwccarm
// coalesces both shifts into their own sources. Mechanism, the levers that are
// measured-closed, and the one open angle: notes/mwccarm-codegen.md 6bn.
// Counts as decompiled, not matched.
#include "types.h"

extern void MulMat4x3Mat4x3(const void *m1, const void *m0, void *mF);
extern void func_020553a4(void *m);
extern void func_ov075_0211a948(void *self);
extern int data_0209b3ec;

void func_ov075_0211afb0(char *self)
{
    int z0;
    int z1;
    int tmp[13];
    int i;
    int j;
    int cols;
    char *base;
    char *p0;
    char *p1;
    int off0;
    int off1;
    int vx;
    int vy;
    int vz;
    int sx;
    int sz;

    *(int *)0x4000444 = 0;
    MulMat4x3Mat4x3((void *)(self + 0x14), &data_0209b3ec, tmp);
    *(int *)0x4000440 = 2;
    func_020553a4(tmp);
    *(int *)0x4000440 = 1;
    func_020553a4(tmp);
    *(int *)0x40004c8 = 0xe0000000;
    *(int *)0x40004cc = 0xc0007fff;
    func_ov075_0211a948(self);

    *(int *)0x400046c = 0x10000;
    *(int *)0x400046c = 0x10000;
    *(int *)0x400046c = 0x10000;

    z0 = 0;
    z1 = 0;
    i = 0;
    if ((int)(*(u8 *)(self + 0xa6) - 1) > 0) {
        do {
            j = z0;
            *(int *)0x4000500 = 2;
            cols = *(u8 *)(self + 0xa7);
            if (cols > 0) {
                do {
                    base = *(char **)(self + 0x80);
                    off0 = (i + j * cols) * 0x18;
                    off1 = (i + 1 + j * cols) * 0x18;
                    p0 = base + off0;
                    p1 = base + off1;
                    *(int *)0x4000488 = *(int *)(p0 + 0x14);
                    j = j + 1;
                    *(int *)0x4000484 = *(int *)(p0 + 0x10);
                    vx = *(int *)(base + off0);
                    vz = *(int *)(p0 + 8);
                    sx = (vx << 9) >> 16;
                    sz = (vz << 9) >> 16;
                    vy = *(int *)(p0 + 4);
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    *(int *)0x4000488 = *(int *)(p1 + 0x14);
                    *(int *)0x4000484 = *(int *)(p1 + 0x10);
                    vx = *(int *)(base + off1);
                    vz = *(int *)(p1 + 8);
                    sx = (vx << 9) >> 16;
                    sz = (vz << 9) >> 16;
                    vy = *(int *)(p1 + 4);
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    cols = *(u8 *)(self + 0xa7);
                } while (j < cols);
            }
            i = i + 1;
            *(int *)0x4000504 = z1;
        } while (i < (int)(*(u8 *)(self + 0xa6) - 1));
    }
    *(int *)0x4000448 = 1;
}
