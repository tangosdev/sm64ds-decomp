//cpp
struct Vector3 { int x, y, z; };

extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020d0948(void *self);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
extern short data_02082214[];
extern int data_ov002_021101b4;
}

extern "C" int func_ov002_020cfbdc(char *self)
{
    Vector3 pts[3];
    char rl[0x78];
    int idx = (unsigned short)(short)(*(short *)(self + 0x8e) + 0x8000) >> 4;
    int r6 = data_02082214[idx * 2];
    int r5 = data_02082214[idx * 2 + 1];

    if (*(unsigned short *)(self + 0x6a6) == 0) {
        if ((*(unsigned char *)(self + 0x6e9) & 2) != 0) {
            if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x380) != 0) {
                void *fr = _ZNK12WithMeshClsn14GetFloorResultEv(self + 0x380);
                if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
                    if (_ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(fr)) != 0) {
                        func_ov002_020d0948(self);
                        return 1;
                    }
                }
            }
        }
    }

    {
        int az = *(int *)(self + 0x64) + (r5 << 5);
        int ay = *(int *)(self + 0x60) - 0xa000;
        int ax = *(int *)(self + 0x5c) + (r6 << 5);
        pts[1].x = ax;
        pts[1].y = ay;
        pts[1].z = az;
    }
    {
        int bx = *(int *)(self + 0x5c) + r6 * 0x60;
        int bz = *(int *)(self + 0x64) + r5 * 0x60;
        int by = *(int *)(self + 0x60) - 0xa000;
        pts[2].x = bx;
        pts[2].y = by;
        pts[2].z = bz;
    }

    _ZN11RaycastLineC1Ev(rl);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &pts[1], &pts[2], self);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        *(int *)((int)(self + 0x5c) & 0xFFFFFFFFFFFFFFFFLL) += r5 * 0x60;
        *(int *)((int)(self + 0x60) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80000;
        *(int *)((int)(self + 0x64) & 0xFFFFFFFFFFFFFFFFLL) += r5 * 0x60;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021101b4);
        _ZN11RaycastLineD1Ev(rl);
        return 1;
    }
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
