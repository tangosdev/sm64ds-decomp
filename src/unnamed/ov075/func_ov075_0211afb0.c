// @symbol func_ov075_0211afb0
/* recovered: draws the VS/wireless entry-menu grid mesh, the sub-object
 * dEntObj_c keeps at +0xe80 (d_ent_obj.cpp passes that address to the init
 * func_ov075_0211b458, which installs this function in its step table).
 *
 * Concatenates the object's own matrix (mMtx) with the pooled camera
 * matrix at data_0209b3ec and pushes the result as both the position and the
 * directional matrix, sets a full-white diffuse/ambient and specular/emission
 * pair, then hands the material state to func_ov075_0211a948. The mesh is a
 * (mGridDim[0] - 1) x mGridDim[1] grid of triangle strips over the 0x18-byte
 * vertex array mVertices: each inner iteration emits vertex (i, j) and
 * vertex (i + 1, j), converting each 1.19.12 coordinate to the VTX_16 format
 * with (v << 9) >> 16 and packing x and y into one command word.
 *
 * Two spellings in the vertex emission are load-bearing for the register
 * colouring of the second emission (notes/mwccarm-codegen.md 6ct): the z
 * coordinate is loaded straight into `sz` and shifted in place, and x << 9 is
 * parked in `cols`, the inner loop's bound, which the loop reloads from
 * mGridDim[1] before its next use. Both keep the shift out of the register of
 * the value it consumes, which is where the ROM's compiler puts it.
 */
#include "types.h"

extern void MulMat4x3Mat4x3(const void *m1, const void *m0, void *mF);
extern void func_020553a4(void *m);
extern void func_ov075_0211a948(void *self);
extern int data_0209b3ec;

/* The mesh fields this file reads. func_ov075_0211b458 sets both grid
 * dimensions to 0x14, sizes the vertex count at 0xa4 as their product and
 * allocates mVertices to match; func_ov075_0211b3b8 frees it. The grid is
 * square, and this file and func_ov075_0211abb0 both use mGridDim[1] as the
 * row stride, so the two bytes are not split into named axes. */
typedef struct EntryMesh {
    u8 pad_00[0x14];
    int mMtx[12]; /* 0x14 -- model matrix, concatenated with the camera */
    u8 pad_44[0x3c];
    char *mVertices; /* 0x80 -- 0x18-byte vertices, mGridDim[0] * mGridDim[1] */
    u8 pad_84[0x22];
    u8 mGridDim[2]; /* 0xa6 */
} EntryMesh;

void func_ov075_0211afb0(char *self)
{
    EntryMesh *mesh = (EntryMesh *)self;
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
    int sx;
    int sz;

    *(int *)0x4000444 = 0;
    MulMat4x3Mat4x3(mesh->mMtx, &data_0209b3ec, tmp);
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
    if ((int)(mesh->mGridDim[0] - 1) > 0) {
        do {
            j = z0;
            *(int *)0x4000500 = 2;
            cols = mesh->mGridDim[1];
            if (cols > 0) {
                do {
                    base = mesh->mVertices;
                    off0 = (i + j * cols) * 0x18;
                    off1 = (i + 1 + j * cols) * 0x18;
                    p0 = base + off0;
                    p1 = base + off1;
                    *(int *)0x4000488 = *(int *)(p0 + 0x14);
                    j = j + 1;
                    *(int *)0x4000484 = *(int *)(p0 + 0x10);
                    vx = *(int *)(base + off0);
                    sz = *(int *)(p0 + 8);
                    cols = vx << 9;
                    sx = cols >> 16;
                    sz = (sz << 9) >> 16;
                    vy = *(int *)(p0 + 4);
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    *(int *)0x4000488 = *(int *)(p1 + 0x14);
                    *(int *)0x4000484 = *(int *)(p1 + 0x10);
                    vx = *(int *)(base + off1);
                    sz = *(int *)(p1 + 8);
                    cols = vx << 9;
                    sx = cols >> 16;
                    sz = (sz << 9) >> 16;
                    vy = *(int *)(p1 + 4);
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    cols = mesh->mGridDim[1];
                } while (j < cols);
            }
            i = i + 1;
            *(int *)0x4000504 = z1;
        } while (i < (int)(mesh->mGridDim[0] - 1));
    }
    *(int *)0x4000448 = 1;
}
