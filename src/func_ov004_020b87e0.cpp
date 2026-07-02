//cpp
struct C;
typedef void (C::*PMF)();

extern PMF data_ov004_020bc974;
extern PMF data_ov004_020bc96c;
extern PMF data_ov004_020bc964;
extern PMF data_ov004_020bc95c;
extern PMF data_ov004_020bc954;
extern PMF data_ov004_020bc94c;
extern PMF data_ov004_020bc944;
extern PMF data_ov004_020bc92c;
extern PMF data_ov004_020bc934;
extern PMF data_ov004_020bc97c;
extern PMF data_ov004_020bc984;
extern PMF data_ov004_020bc99c;
extern PMF data_ov004_020bc9a4;
extern PMF data_ov004_020bca3c;
extern PMF data_ov004_020bc9bc;
extern PMF data_ov004_020bc9d4;
extern PMF data_ov004_020bc9dc;
extern PMF data_ov004_020bc9ec;
extern PMF data_ov004_020bc9f4;
extern PMF data_ov004_020bca0c;

extern PMF data_02086b58;

struct C {
    PMF onUpdate;      // +0
    int field_8;       // +8
    int field_c;       // +0xc
    PMF onSomething;   // +0x10
    int index;         // +0x18
};

extern "C" void func_ov004_020b87e0(C* self, int idx) {
    static PMF table[20] = {
        data_ov004_020bc974,
        data_ov004_020bc96c,
        data_ov004_020bc964,
        data_ov004_020bc95c,
        data_ov004_020bc954,
        data_ov004_020bc94c,
        data_ov004_020bc944,
        data_ov004_020bc92c,
        data_ov004_020bc934,
        data_ov004_020bc97c,
        data_ov004_020bc984,
        data_ov004_020bc99c,
        data_ov004_020bc9a4,
        data_ov004_020bca3c,
        data_ov004_020bc9bc,
        data_ov004_020bc9d4,
        data_ov004_020bc9dc,
        data_ov004_020bc9ec,
        data_ov004_020bc9f4,
        data_ov004_020bca0c,
    };
    self->index = idx;
    self->onUpdate = table[self->index];
    self->onSomething = data_02086b58;
    if (self->onUpdate) (self->*self->onUpdate)();
}
