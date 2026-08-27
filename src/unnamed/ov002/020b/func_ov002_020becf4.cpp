//cpp
extern "C" int _ZN6Player8HasNoCapEv(void* self);

extern "C" int func_ov002_020becf4(char* self, int v, int arg2){
    int cond;
    int r1;
    if (arg2 == 0) {
        if (*(unsigned char*)(self + 0x6f9)) v = 9;
        if (*(unsigned char*)(self + 0x6fd)) v = 8;
        cond = _ZN6Player8HasNoCapEv(self);
    } else {
        if (*(unsigned char*)(self + 0x6fa)) v = 9;
        if (*(unsigned char*)(self + 0x6fe)) v = 8;
        cond = *(unsigned char*)(self + 0x701);
    }
    r1 = 0;
    if ((unsigned int)v >= 4) {
        if (cond) r1 = 1;
    } else if (v == 3) {
        if (*(unsigned char*)(self + 0x714)) r1 = 4;
    } else {
        if (cond) r1 = 4;
    }
    return (r1 + v) & 0xff;
}
