//cpp
struct Obj;
typedef void (Obj::*PMF)();

extern PMF data_ov002_02110f9c[];
extern "C" void func_ov002_020f2e30(Obj *self, unsigned int idx);

extern "C" void func_ov002_020f5f0c(Obj *self, unsigned int idx)
{
    (self->*data_ov002_02110f9c[idx])();
    func_ov002_020f2e30(self, idx);
}
