//cpp
extern "C" int func_02012694(int a, int* b);
extern "C" int Bowser_IsAnimAtLastFrame(void* o);
extern "C" void func_ov060_02111cc0(void* o, int a, int b);
extern "C" int _ZNK9Animation12WillHitFrameEi(void* self, int frame);

extern int data_ov060_0211ac20[];
extern int data_ov060_0211ac68[];
extern int data_ov060_0211ac70[];

extern "C" void func_ov060_02114e9c(char* c)
{
    *(int*)(c + 0x98) = 0;
    if (*(int*)(c + 0x134) == data_ov060_0211ac20[1]) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 8)) {
            func_02012694(0xb5, (int*)(c + 0x74));
        }
    }
    if (Bowser_IsAnimAtLastFrame(c) == 0) return;
    if (*(int*)(c + 0x134) == data_ov060_0211ac20[1]) {
        func_ov060_02111cc0(c, 6, 0x40000000);
        return;
    }
    if (*(int*)(c + 0x134) == data_ov060_0211ac68[1]) {
        func_ov060_02111cc0(c, 7, 0x40000000);
        return;
    }
    if (*(int*)(c + 0x134) == data_ov060_0211ac70[1]) {
        *(int*)(c + 0x40c) = 0;
        return;
    }
    func_ov060_02111cc0(c, 8, 0x40000000);
}
