int func_ov006_021108bc(int* a, int* b);

int func_ov006_02112030(char* c, int* arg){
    int buf[2];
    int i = 0;
    int* zero = 0;
    while (i < *(int*)(*(char**)(c+4) + 0x4000 + 0x678)) {
        int* p;
        char* base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = *(char**)(c+4);
        p = (i >= *(int*)(base + 0x4000 + 0x678)) ? zero : ((int**)(base + 0x4000 + 0x74c))[i];
        if (func_ov006_021108bc(p, buf)) return 1;
        i++;
    }
    return 0;
}
