//cpp
extern "C" int _ZN6Player6ST_OWLE(int* a, int* b);

extern "C" int func_ov006_02111f8c(char* c, int* arg){
    int buf[2];
    int i = 0;
    int* zero = 0;
    while (i < *(int*)(*(char**)(c+4) + 0x4000 + 0x67c)) {
        int* p;
        char* base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = *(char**)(c+4);
        p = (i >= *(int*)(base + 0x4000 + 0x67c)) ? zero : ((int**)(base + 0x4000 + 0x764))[i];
        if (_ZN6Player6ST_OWLE(p, buf)) return 1;
        i++;
    }
    return 0;
}
