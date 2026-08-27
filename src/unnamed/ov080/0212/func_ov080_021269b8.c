extern unsigned short DecIfAbove0_Short(unsigned short* p);
void func_ov080_021256f8(char* c);
int func_ov080_02125bb0(char* c, int x);
void func_ov080_02125940(char* c);
void func_ov080_02125af0(char* c);

struct Elem { char pad[8]; int f8; int fc; char pad2[0xc]; };

void func_ov080_021269b8(char* c)
{
    int i;
    short n;
    struct Elem* arr;

    func_ov080_021256f8(c);
    if (DecIfAbove0_Short((unsigned short*)(c + 0x1b6)) == 0) return;

    for (i = 0; i < *(unsigned short*)(c + 0x1b8); i++) {
        arr = (struct Elem*)(*(int*)(c + 0x1a0) + i * 0x18);
        arr->f8 = func_ov080_02125bb0(c, arr->fc);
    }

    func_ov080_02125940(c);
    func_ov080_02125af0(c);

    n = *(short*)(c + 0x1b4);
    *(short*)(c + 0x1b4) = n + *(int*)(*(int*)(c + 0x1ac) + 4);
}
