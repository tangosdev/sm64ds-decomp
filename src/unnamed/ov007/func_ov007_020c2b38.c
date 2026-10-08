extern void* func_ov007_020c2b9c(int a, int b);
extern void func_ov007_020c2d44(void* obj, int i);

void* func_ov007_020c2b38(int count, int* src1, int* src2, int arg3) {
    void* obj = func_ov007_020c2b9c(count, arg3);
    int i;
    for (i = 0; i < count; i++) {
        (*(int**)((char*)obj + 0x24))[i] = src1[i];
        (*(int**)((char*)obj + 0x28))[i] = src2[i];
        func_ov007_020c2d44(obj, i);
    }
    return obj;
}
