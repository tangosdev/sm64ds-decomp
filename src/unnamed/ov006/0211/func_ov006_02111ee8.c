int func_ov006_0210e1fc(int* a, int* b);

int func_ov006_02111ee8(char* c, int* arg){
    int buf[2];
    int i = 0;
    int* zero = 0;
    while (i < *(int*)(*(char**)(c+4) + 0x4000 + 0x680)) {
        int* p;
        char* base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = *(char**)(c+4);
        p = (i >= *(int*)(base + 0x4000 + 0x680)) ? zero : ((int**)(base + 0x4000 + 0x770))[i];
        if (func_ov006_0210e1fc(p, buf)) return 1;
        i++;
    }
    return 0;
}
