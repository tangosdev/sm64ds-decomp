//cpp
struct Class {
    char pad[0x44c];
    int sel;
};

struct Entry {
    void (Class::*pmf)();
    int extra[3];
};

extern Entry data_ov002_0210af2c[];

extern "C" void func_ov002_020efa54(Class *self, int sel) {
    self->sel = sel;
    int idx = self->sel;
    (self->*(data_ov002_0210af2c[idx].pmf))();
}
