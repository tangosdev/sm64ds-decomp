//cpp
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
extern u8 data_020a0e40;
extern u8 data_0209f49e[];
extern State data_ov002_02110574;
extern State data_ov002_02110634;
extern State data_ov002_0211061c;
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *self, State &s);
extern "C" int Player_ScaleByCharFactor(void *c, int a);

extern "C" int func_ov002_020d12b0(u8 *self)
{
    if ((*(u16 *)(data_0209f49e + data_020a0e40 * 0x18) & 1) == 0) goto ret0;
    if (self[0x703] != 0) {
        self[0x6e2] = 2;
        _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_02110574);
    } else {
        if (*(int *)(self + 0x98) >= Player_ScaleByCharFactor(self, 0xa000)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_02110634);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_0211061c);
        }
    }
    return 1;
ret0:
    return 0;
}
