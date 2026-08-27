extern void* func_ov007_020c3df4(int a, int b);
extern void* func_ov007_020c9388(void* self);

void* func_ov007_020ca2a8(void* arg) {
    int* obj = (int*)func_ov007_020c3df4(0, 0x1c);
    int i;
    for (i = 0; i < 4; i++) {
        (obj + i)[2] = (int)func_ov007_020c9388((void*)-2);
    }
    obj[6] = (int)arg;
    obj[1] = 0;
    obj[0] = 0;
    return obj;
}
