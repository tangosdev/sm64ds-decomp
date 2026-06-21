//cpp
struct Class {
    char pad[0x15c];
    unsigned char guard;
    unsigned char idx;
};

extern void (Class::*data_ov002_02110e4c[])(int);

extern "C" void func_ov002_020f37a0(Class *self) {
    if (self->guard == 0) return;
    (self->*(data_ov002_02110e4c[self->idx]))(0);
}
