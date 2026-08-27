typedef int s32;

struct Matrix4x3 {
    int data[12]; /* 0x30 bytes */
};

struct dBase_c {
    char pad[0x50];
};

struct View {
    struct dBase_c base; /* 0x00 - 0x50 */
    struct Matrix4x3 camMat;  /* 0x50 */
};

extern void _Z13CopyToViewMatPK9Matrix4x3(const struct Matrix4x3* mat);

s32 _ZN4View6RenderEv(struct View* self) {
    _Z13CopyToViewMatPK9Matrix4x3(&self->camMat);
    return 1;
}
