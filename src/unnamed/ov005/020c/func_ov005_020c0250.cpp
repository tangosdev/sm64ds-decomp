//cpp
extern "C" {
int func_02012790(int);
extern void* data_0209f5bc;
extern unsigned char data_020a0e40;
struct S { unsigned char a, b, c, d; };
extern struct S data_020a0de8[];

void func_ov005_020c0250(char* c) {
    void* g = data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0) return;
    if (*(unsigned char*)(c+0xac) != 0) return;
    if (*(unsigned char*)(c+0x54) == 1) return;
    if (*(int*)(c+0x90) > 0) return;
    if (*(int*)(c+0x94) > 0) return;
    if (*(int*)(c+0x98) > 0) return;
    unsigned int i = data_020a0e40;
    bool ok = false;
    if (data_020a0de8[i].a != 0) {
        if (data_020a0de8[i].b != 0) ok = true;
    }
    if (!ok) return;
    unsigned char x = data_020a0de8[i].c;
    if (x < 0xd8) return;
    if (x > 0xf8) return;
    unsigned char y = data_020a0de8[i].d;
    if (y < 0x90) return;
    if (y > 0xb0) return;
    func_02012790(0x63);
    *(int*)(c+0x98) = 0x1c;
}
}
