//cpp
extern "C" {
int _ZN6Player15IsCollectingCapEv(void* p);
extern signed char data_02092114[];
extern unsigned char data_ov001_020ad628[];
extern unsigned char data_ov001_020ad62c[];
extern void* data_0209f394[];

int func_ov001_020aa858(int self, char* a, int b, int d)
{
    if (b == 0 && self == data_02092114[0]) {
        data_ov001_020ad628[self] |= 0x40;
        data_ov001_020ad62c[self] |= 0x80;
        return 0;
    }
    if (a == 0) return 0;
    unsigned char k = *(unsigned char*)(a+0x19);
    if (k >= 3) return 1;
    if (d != 0) return 0;
    void* p = data_0209f394[0];
    if (k != 4) {
        if (_ZN6Player15IsCollectingCapEv(p)) return 0;
    }
    if (!_ZN6Player15IsCollectingCapEv(p)) {
        if (self == *(int*)((char*)p+8)) return 0;
    }
    return 1;
}
}
