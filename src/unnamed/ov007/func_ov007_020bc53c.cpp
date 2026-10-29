//cpp
extern "C" void func_ov007_020c92a0(int);
extern "C" void func_ov007_020c3d1c(int);

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in that
   file, only the slots this function frees. The four animation players go
   back through func_ov007_020c92a0, the buffers through func_ov007_020c3d1c.
   +0x84 is func_ov007_020bc6d4's materialTexParams (func_ov007_020bc47c
   fills it); this view leaves it unnamed and frees it raw through `r4`. */
struct Ov007Model {
    void *model;          /* 0x00 */
    void *transforms;     /* 0x04 */
    void *nextTransforms; /* 0x08: flags & 2 only */
    void *blendBuffer;    /* 0x0c: flags & 4 only */
    char pad_10[8];
    void *boneAnim; /* 0x18 */
    char pad_1c[8];
    void *texSrtAnim; /* 0x24 */
    char pad_28[8];
    void *texSeqAnim; /* 0x30 */
    char pad_34[0x54];
    void *blendAnim; /* 0x88: flags & 4 only */
};

typedef char Ov007Model_size_must_be_0x8c[sizeof(Ov007Model) == 0x8c ? 1 : -1];

extern "C" void func_ov007_020bc53c(int r0) {
    int *r4 = (int *)r0;
    Ov007Model *m = (Ov007Model *)r0;
    if (m->boneAnim) {
        func_ov007_020c92a0((int)m->boneAnim);
        m->boneAnim = 0;
    }
    if (m->texSrtAnim) {
        func_ov007_020c92a0((int)m->texSrtAnim);
        m->texSrtAnim = 0;
    }
    if (m->texSeqAnim) {
        func_ov007_020c92a0((int)m->texSeqAnim);
        m->texSeqAnim = 0;
    }
    if (m->blendAnim) {
        func_ov007_020c92a0((int)m->blendAnim);
        m->blendAnim = 0;
    }
    if (m->blendBuffer) {
        func_ov007_020c3d1c((int)m->blendBuffer);
        m->blendBuffer = 0;
    }
    if (m->nextTransforms) {
        func_ov007_020c3d1c((int)m->nextTransforms);
        m->nextTransforms = 0;
    }
    func_ov007_020c3d1c(r4[0x84 / 4]);
    func_ov007_020c3d1c((int)m->transforms);
    func_ov007_020c3d1c((int)m->model);
    func_ov007_020c3d1c((int)m);
}
