typedef int Fix12;

extern void _ZN9Animation7AdvanceEv(void* a);
extern int _ZN9Animation8FinishedEv(void* a);
extern void _ZN6Player16SetRealCharacterEj(void* p, unsigned int ch);
extern void func_0201277c(int a);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, Fix12 b);
extern void func_02012790(int a);
extern void func_02012694(int a, void* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern int func_ov100_021451c4(char* c, void* r1, char* r4);
extern void _ZN6Player11OpenBigDoorEv(void* p);

extern unsigned char data_0209caa0[];
extern unsigned char data_ov100_021480d0[];
extern unsigned char data_ov100_02148704[];
extern int data_ov100_021488f4[];

int func_ov100_02144528(char* c, char* pl)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (_ZN9Animation8FinishedEv(c + 0x124) != 0) {
        if (*(unsigned char*)(c + 0x145) == 0) {
            if (*(int*)(c + 8) != 0xd) {
                unsigned int ch = data_ov100_021480d0[*(signed char*)(c + 0x144) - 1];
                if (ch == data_0209caa0[0x41])
                    ch = 3;
                _ZN6Player16SetRealCharacterEj(pl, ch);
                if (data_ov100_02148704[0] != 0) {
                    *(unsigned char*)(pl + 0x71a) = 1;
                    func_0201277c(0x7e);
                    *(unsigned char*)(c + 0x145) = 0x40;
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x19666);
                } else {
                    func_0201277c(0x7f);
                }
            } else {
                *(unsigned char*)(c + 0x145) = 0x40;
                if (data_ov100_02148704[0] != 0)
                    func_02012790(0xa1);
                else
                    func_02012790(0xa2);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x19666);
            }
            func_02012694(*(int*)(c + 8) == 0x10 ? 7 : 5, c + 0x74);
        }
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x145)) == 0) {
            void* r1 = data_ov100_02148704[0] != 0 ? 0 : (void*)data_ov100_021488f4;
            if (func_ov100_021451c4(c, r1, pl) != 0) {
                if (data_ov100_02148704[0] != 0)
                    _ZN6Player11OpenBigDoorEv(pl);
            } else {
                unsigned char* q = (unsigned char*)(((int)c + 0x145) & 0xFFFFFFFFFFFFFFFF);
                *q += 1;
            }
        } else {
            if (data_ov100_02148704[0] == 0
                && *(unsigned char*)(c + 0x145) == 0xa
                && *(int*)(c + 8) == 0xd) {
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0xcb33);
            }
        }
    }
    return 1;
}
