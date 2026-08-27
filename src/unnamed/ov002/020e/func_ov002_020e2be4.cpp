//cpp
extern "C" int func_ov002_020c04ac(char* c);
extern "C" int func_ov002_020c031c(void* c);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void* self, void* state);

extern char data_ov002_021103f4;

extern "C" int func_ov002_020e2be4(char* self){
    int thr;
    if (func_ov002_020c04ac(self))
        return 0;
    switch (func_ov002_020c031c(self)) {
    case 4:
    case 5:
        thr = 0xf74;
        break;
    case 3:
        thr = 0xf04;
        break;
    default:
        thr = 0xddb;
        break;
    case 1:
    case 2:
        thr = 0xddb;
        break;
    }
    if (*(int*)(self + 0x558) > thr)
        return 0;
    _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021103f4);
    return 1;
}
