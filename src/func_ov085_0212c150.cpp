//cpp
struct Mtx { int w[12]; };
extern "C" {
extern void func_0203c178(void *m, int a, int b, int c);
extern void MulMat3x3Mat3x3(void *a, void *b, void *c);
extern void func_02016acc(void *p, int v);
extern void func_02016b24(void *p, int v);
extern int MATRIX_SCRATCH_PAPER;
extern void func_ov085_0212c150(void *self);
}
extern "C" void _ZN9ModelAnim6RenderEPK7Vector3(void *self, void *v);
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int a, unsigned int b);
void func_ov085_0212c150(void *self) {
    char *c = (char *)self;
    Mtx tmp;
    tmp = *(Mtx *)(c + 0x31c);
    *(int *)(c + 0x340) = 0x421800 - *(int *)(c + 0x340);
    func_0203c178(&MATRIX_SCRATCH_PAPER, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((void *)(c + 0x31c), &MATRIX_SCRATCH_PAPER, (void *)(c + 0x31c));
    func_02016acc((void *)(c + 0x300), 0x80);
    func_02016b24((void *)(c + 0x300), 0x40);
    _ZN9ModelAnim6RenderEPK7Vector3((void *)(c + 0x300), (void *)(c + 0x80));
    _ZN9ModelBase12ApplyOpacityEj((void *)(c + 0x300), 0xff, 0);
    func_02016b24((void *)(c + 0x300), 0x80);
    func_02016acc((void *)(c + 0x300), 0x40);
    *(Mtx *)(c + 0x31c) = tmp;
}
