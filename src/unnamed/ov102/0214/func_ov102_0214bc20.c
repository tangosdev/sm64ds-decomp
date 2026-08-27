extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov102_0214e9c0;

typedef struct { int v[4]; } Quad;
extern Quad data_ov102_0214e514;
extern Quad data_ov102_0214e524;

void func_ov102_0214bc20(char* c)
{
    Quad a1 = data_ov102_0214e514;
    Quad a2 = data_ov102_0214e524;
    void *p = *(void**)(c + 0x390);
    int idx = 0;
    if (p != 0) {
        idx = *(int*)((char*)p + 8);
        if ((unsigned)idx > 4) idx = 4;
    }
    *(int*)(c + 0x98) = a1.v[idx];
    *(int*)(c + 0xa8) = a2.v[idx];
    *(int*)(c + 0x3dc) = 4;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov102_0214e9c0.w[1], 0, 0x1000, 0);
    *(int*)(c + 0x9c) = -0x2000;
}
