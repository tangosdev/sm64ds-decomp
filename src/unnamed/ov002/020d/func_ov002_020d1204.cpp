//cpp
struct State;
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void*, State&);
extern unsigned char data_020a0e40[];
extern unsigned char data_0209f49e[];
extern State data_ov002_0211052c;
extern State data_ov002_0211055c;
extern State data_ov002_0211019c;

extern "C" int func_ov002_020d1204(void* thisptr)
{
    unsigned char* p = (unsigned char*)thisptr;
    if (*(unsigned short*)(data_0209f49e + data_020a0e40[0] * 0x18) & 2) {
        int v = *(int*)(p + 0x98);
        if (v == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, data_ov002_0211052c);
        } else if (*(unsigned short*)(p + 0x600 + 0xa4) != 0 && v > 0xa000) {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, data_ov002_0211055c);
            return 1;
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, data_ov002_0211019c);
        }
        return 1;
    }
    return 0;
}
