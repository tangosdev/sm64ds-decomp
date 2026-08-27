//cpp
struct Vec4 { int a, b, c, d; ~Vec4(){} };
extern "C" {
char* _ZN8dActor_c13ClosestPlayerEv(void);
void func_ov066_02119454(char* c, void* p);
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae04;
extern int data_ov066_0211b0ac;

int func_ov066_02119398(char* c)
{
    Vec4 sp;
    char* p = _ZN8dActor_c13ClosestPlayerEv();
    if (p != 0) {
        char* sp2 = p + 0x5c;
        int v1 = *(int*)(sp2 + 4);
        int v2 = *(int*)(sp2 + 8);
        if (v1 < -0x300000) {
            int f = (int)((*(int*)(c + 0xb0) & 8) != 0);
            if (f == 0) {
                if (v2 < -0xd70000) {
                    data_ov066_0211ae08 += 1;
                }
            }
        }
    }
    if (data_ov066_0211ae08 > 2) {
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae04 = 2;
        func_ov066_02119454(c, &data_ov066_0211b0ac);
    }
    return 1;
}
}
