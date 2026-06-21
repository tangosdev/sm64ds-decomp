//cpp
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File *, int, int, unsigned int);
};
struct Obj {
    char pad[0x300];
    ModelAnim anim;
};
extern BCA_File *data_ov002_0210eb78[];

extern "C" void func_ov002_020ec978(Obj *self) {
    self->anim.SetAnim(data_ov002_0210eb78[1], 0, 0x1000, 0);
    *(int*)((char*)self + 0x98) = 0;
    *(int*)((char*)self + 0x9c) = -0x2000;
}
