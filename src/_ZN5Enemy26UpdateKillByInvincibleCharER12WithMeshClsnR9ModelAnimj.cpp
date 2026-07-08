//cpp
// NONMATCHING: extra logic (you do more) (div=64). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct WithMeshClsn;
struct ModelAnim;
struct CylinderClsn;

extern "C" int  WMC_IsOnGround(void *self);
extern "C" int  WMC_IsOnWall(void *self);
extern "C" void Vec3_Asr(void *d, void *s, int sh);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);

extern int MATRIX_SCRATCH_PAPER;
struct M48 { int w[12]; };

struct Enemy {
    int SpawnCoin();
    int KillAndTrackInDeathTable();
    int UpdatePos(CylinderClsn *c);
    int UpdateWMClsn(WithMeshClsn &w, unsigned int j);
    short ReflectAngle(int a, int b, short s);
    int UpdateKillByInvincibleChar(WithMeshClsn &w, ModelAnim &m, unsigned int j);
};

struct VT {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  vm();
};

int Enemy::UpdateKillByInvincibleChar(WithMeshClsn &w, ModelAnim &m, unsigned int j)
{
    unsigned char *c = (unsigned char *)this;
    unsigned char *r6 = (unsigned char *)&w;
    unsigned char *r5 = (unsigned char *)&m;

    if (*(int *)(c + 0x10c) != 8)
        return 0;

    if (*(unsigned short *)(c + 0x102) != 0) {
        unsigned short *p = (unsigned short *)(c + 0x102);
        *p = *p - 1;
    }

    if (*(unsigned short *)(c + 0x102) != 0) {
        if (r6 != 0 && WMC_IsOnGround(r6) != 0 && *(int *)(c + 0xa8) < 0) {
            if (j & 1)
                this->SpawnCoin();
            if (j & 2)
                this->KillAndTrackInDeathTable();
            *(int *)(c + 0x10c) = 0;
            return 2;
        }
    }

    this->UpdatePos(0);
    if (r6 != 0) {
        this->UpdateWMClsn(w, 0);
        if (WMC_IsOnWall(r6) != 0) {
            *(short *)(c + 0x94) = this->ReflectAngle(*(int *)(c + 0xe0), *(int *)(c + 0xe8),
                                                      *(short *)(c + 0x94));
        }
    }

    if (r5 != 0) {
        int t[3];
        short *a8c = (short *)(c + 0x8c);
        short *a8e = (short *)(c + 0x8e);
        short *a90 = (short *)(c + 0x90);
        *a8c = (short)(*a8c + *(short *)(c + 0xec));
        *a8e = (short)(*a8e + *(short *)(c + 0xee));
        *a90 = (short)(*a90 + *(short *)(c + 0xf0));
        Vec3_Asr(t, c + 0x5c, 3);
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, t[0], t[1], t[2]);
        {
            int r = ((VT *)c)->vm();
            Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, r >> 3, 0);
        }
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&MATRIX_SCRATCH_PAPER,
                                               *(short *)(c + 0x8c), *(short *)(c + 0x8e),
                                               *(short *)(c + 0x90));
        {
            int r = ((VT *)c)->vm();
            Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, (-r) >> 3, 0);
        }
        *(M48 *)(r5 + 0x1c) = *(M48 *)&MATRIX_SCRATCH_PAPER;
    }
    return 1;
}
