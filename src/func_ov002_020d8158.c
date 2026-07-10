typedef unsigned short u16;
typedef unsigned char u8;

struct State;
extern int _ZN6Player7IsStateERNS_5StateE(void* self, struct State* s);
extern void func_ov002_020d8118(char* c);
extern void func_ov002_020bdc18(char* c);
extern struct State data_ov002_02110034;
extern struct State data_ov002_0211004c;

void func_ov002_020d8158(char* c)
{
    if (*(int*)(c + 8) != 3) {
        *(u16*)(int)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x1000;
        return;
    }
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110034)) return;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211004c)) return;
    if (*(u8*)(c + 0x709) != 0) return;
    if ((u16)(*(u16*)(c + 0x6ce) & 0x1000) == 0) return;
    func_ov002_020d8118(c);
    *(u8*)(c + 0x6f4) = 1;
    func_ov002_020bdc18(c);
    *(u16*)(int)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x1000;
    *(u8*)(c + 0x714) = 1;
}
