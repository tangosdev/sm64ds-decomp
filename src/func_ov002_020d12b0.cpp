//cpp
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
extern u8 data_020a0e40;
extern u8 data_0209f49e[];
extern State _ZN6Player13ST_PUNCH_KICKE;
extern State _ZN6Player13ST_SLIDE_KICKE;
extern State _ZN6Player13ST_SWEEP_KICKE;
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *self, State &s);
extern "C" int func_ov002_020bf30c(void *c, int a);

extern "C" int func_ov002_020d12b0(u8 *self)
{
    if ((*(u16 *)(data_0209f49e + data_020a0e40 * 0x18) & 1) == 0) goto ret0;
    if (self[0x703] != 0) {
        self[0x6e2] = 2;
        _ZN6Player11ChangeStateERNS_5StateE(self, _ZN6Player13ST_PUNCH_KICKE);
    } else {
        if (*(int *)(self + 0x98) >= func_ov002_020bf30c(self, 0xa000)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, _ZN6Player13ST_SLIDE_KICKE);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(self, _ZN6Player13ST_SWEEP_KICKE);
        }
    }
    return 1;
ret0:
    return 0;
}
