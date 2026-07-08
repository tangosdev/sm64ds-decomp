//cpp
// NONMATCHING: different op / idiom (div=43). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *o);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *o);
extern void _ZN5Enemy9SpawnCoinEv(void *o);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *o);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *o, void *cc);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *o, void *w, unsigned j);
extern void Vec3_Asr(void *d, void *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern char MATRIX_SCRATCH_PAPER;
}

struct VC {
    int vt_dummy[29];
    int slot29();
};
struct M48 { int w[12]; };

extern "C" int func_ov002_020ad660(void *cc, void *pp, void *r5p, int flags)
{
    char *c = (char*)cc;
    char *p = (char*)pp;
    char *r5 = (char*)r5p;
    int v[3];

    if (*(int*)(c + 0x10c) != 8)
        return 0;

    if (*(unsigned short*)(c + 0x102) == 0 ||
        (p != 0 && (_ZNK12WithMeshClsn10IsOnGroundEv(p) || _ZNK12WithMeshClsn8IsOnWallEv(p)))) {
        if (flags & 1)
            _ZN5Enemy9SpawnCoinEv(c);
        if (flags & 2)
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        *(int*)(c + 0x10c) = 0;
        return 2;
    }

    *(unsigned short*)(c + 0x102) -= 1;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (p != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, p, 0);

    if (r5 != 0) {
        *(short*)(c + 0x8c) += *(short*)(c + 0xec);
        *(short*)(c + 0x8e) += *(short*)(c + 0xee);
        *(short*)(c + 0x90) += *(short*)(c + 0xf0);
        Vec3_Asr(v, c + 0x5c, 3);
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);

        {
            VC *o = (VC*)c;
            int r = o->slot29();
            Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, r >> 3, 0);
        }
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&MATRIX_SCRATCH_PAPER,
            *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
        {
            VC *o = (VC*)c;
            int r = o->slot29();
            Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, (-r) >> 3, 0);
        }
        *(M48*)(r5 + 0x1c) = *(M48*)&MATRIX_SCRATCH_PAPER;
    }
    return 1;
}
