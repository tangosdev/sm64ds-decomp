/* func_ov002_020e9840 at 0x020e9840 (ov002), size 0x1a8
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
struct Vector3 { int x, y, z; };

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020e9590(char *self);
extern int func_ov002_020e8dd8(unsigned char *self);
extern void func_02012694(unsigned int id, const struct Vector3 *v);
extern void func_ov002_020e9448(unsigned char *p);
extern s16 GetAngleToCamera(int i);

extern u8 data_0209f208;
extern u8 *data_0209f344;
extern u8 data_0209f2d8;

void func_ov002_020e9840(char *self)
{
    char *actor;

    actor = (char *)_ZN5Actor10FindWithIDEj(*(unsigned int *)(self + 0x434));
    if (actor == 0) {
        func_ov002_020e9590(self);
        return;
    }
    if (actor == 0) return;

    if ((unsigned)(*(u16 *)(self + 0x4a2) << 30) >> 31 == 0) {
        if (*(u8 *)(self + 0x49d) != data_0209f344[data_0209f208]) return;
        if (*(u16 *)(actor + 0x1d4) != 0) return;

        *(u16 *)(((long long)(int)(self + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        func_ov002_020e8dd8((unsigned char *)self);
        func_02012694(0x54, (struct Vector3 *)(self + 0x74));
        return;
    }

    if ((unsigned)(*(u8 *)(actor + 0x1db) << 31) >> 31 == 0) return;

    *(u16 *)(((unsigned long long)(unsigned int)(self + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL) &= ~8;
    *(u16 *)(self + 0x100) = 0xf;
    *(int *)(self + 0x440) = 8;
    *(int *)(self + 0xa8) = 0x20000;
    func_ov002_020e9448((unsigned char *)self);

    *(int *)(((long long)(int)(self + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;

    {
        int r0 = (data_0209f2d8 == 1) ? 1 : 0;
        int *r3 = *(int **)(actor + 0x1d0);
        if (r0 == 0) return;
        if (r3 == 0) return;

        *(u16 *)(((long long)(int)(self + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
        *(int *)(self + 0x98) = 0xc000;
        *(s16 *)(self + 0x94) = GetAngleToCamera(*(u8 *)((char *)r3 + 0x6d8)) + 0x8000;
    }
}
