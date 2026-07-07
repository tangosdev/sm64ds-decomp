typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int Fix12i;

struct Mtx43 { int w[12]; };

extern int func_ov002_020e3f90(void);
extern void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Mtx43 *mF, s16 angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Mtx43 *mF, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(struct Mtx43 *mF, s16 angZ);
extern int _ZNK6Player14GetBodyModelIDEjb(void *self, u32 unk, int b);
extern int func_ov002_020becf4(void *self, u32 v, int arg2);
extern int func_ov002_020d225c(void *o);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Mtx43 *mF, Fix12i x, Fix12i y, Fix12i z);
extern void func_ov002_020e4374(void *c, int *p1, int *p2);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *mtx, Fix12i a, Fix12i b, u32 flags);

extern struct Mtx43 data_020a0e68;

void func_ov002_020e444c(char *c)
{
    int sp8, spC;
    int b;

    if (func_ov002_020e3f90() == 0) {
        int y = *(int *)(c + 0x690) + (*(int *)(c + 0x60) - *(int *)(c + 0x68c)) + 0xf000;
        if (*(u8 *)(c + 0x6fd) != 0)
            y -= 0x70000;
        Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c) >> 3, y >> 3, *(int *)(c + 0x64) >> 3);
        *(struct Mtx43 *)(c + 0x5bc) = data_020a0e68;

        {
            int out[3];
            int src[3];
            int v694 = *(int *)(c + 0x694);
            src[1] = *(int *)(c + 0x690) - *(int *)(c + 0x68c);
            src[2] = v694;
            src[0] = 0;
            Vec3_RotateYAndTranslate(out, (int *)(c + 0x5c), *(s16 *)(c + 0x8e), src);
            Matrix4x3_FromTranslation(&data_020a0e68, out[0] >> 3, out[1] >> 3, out[2] >> 3);
        }

        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + 0x90));

        if (*(u8 *)(c + 0x6fd) == 0) {
            int bodyID = _ZNK6Player14GetBodyModelIDEjb(c, *(u32 *)(c + 8) & 0xff, 0);
            char *p = *(char **)(c + bodyID * 4 + 0xdc);
            *(struct Mtx43 *)(p + 0x1c) = data_020a0e68;
            *(struct Mtx43 *)(c + 0x190) = data_020a0e68;
        } else {
            *(struct Mtx43 *)(c + 0x10c) = data_020a0e68;
        }

        {
            int r0 = func_ov002_020becf4(c, *(u32 *)(c + 8) & 0xff, 0);
            if (r0 != 9 && r0 != 8) {
                char *q = *(char **)(c + r0 * 4 + 0x154);
                if (q != 0) {
                    *(struct Mtx43 *)(q + 0x1c) = data_020a0e68;
                }
            }
        }

        if (func_ov002_020d225c(c) != 0) {
            int z = (int)(((long long)(*(int *)(c + 0x574)) * 0x2400 + 0x800) >> 12) + 0xc000;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x7000, z);
            char *p = *(char **)(c + 0x1d8);
            *(struct Mtx43 *)(p + 0x1c) = data_020a0e68;
        }
    }

    func_ov002_020e4374(c, &sp8, &spC);

    if ((*(u8 *)(c + 0x6f5) | *(u8 *)(c + 0x6fb)) == 0)
        *(u8 *)(c + 0x717) = 1;

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380) != 0) {
        void *fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x380);
        if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
            void *a = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(fr));
            if (a != 0) {
                b = (int)(*(u16 *)((char *)a + 0xc) == 0xa1);
                if (b != 0) {
                    sp8 += 0x3c000;
                }
            }
        }
    }

    if (*(u8 *)(c + 0x717) != 0)
        return;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x2ac, c + 0x5bc, spC, sp8, 0xf);
}
