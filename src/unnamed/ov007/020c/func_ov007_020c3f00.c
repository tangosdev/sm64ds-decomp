extern int func_ov007_020c3df4(int a, void* b);
extern void * func_ov007_020c11ac(int n);
extern void* func_ov007_020c40d4(void* r5);
extern void func_ov007_020c10b0(void* a, int b);
extern void func_ov007_020c4090(char* c);

void* func_ov007_020c3f00(int* r6, void* r4)
{
    char* r5 = (char*)func_ov007_020c3df4(0, (void*)0x18);
    *(int**)r5 = r6;
    *(void**)(r5 + 4) = func_ov007_020c11ac(0);
    *(unsigned char*)(*(char**)(r5 + 4) + 0x14) = 4;
    *(int*)(*(char**)(r5 + 4) + 8) = 0;
    *(void**)(r5 + 0xc) = func_ov007_020c40d4(r4);
    func_ov007_020c10b0(*(void**)(r5 + 4), *(int*)*(void**)(r5 + 0xc));
    *(void**)(r5 + 8) = (void*)func_ov007_020c3df4(0, (void*)(*r6 << 2));
    int i = 0;
    while (i < *r6) {
        ((void**)*(char**)(r5 + 8))[i] = func_ov007_020c40d4(*(void**)((char*)r6 + 8));
        char* r7 = (char*)((void**)*(char**)(r5 + 8))[i];
        func_ov007_020c4090(r7);
        func_ov007_020c10b0(*(void**)(r5 + 4), *(int*)r7);
        i++;
    }
    *(int*)(r5 + 0x14) = 0;
    *(int*)(r5 + 0x10) = -1;
    return r5;
}
