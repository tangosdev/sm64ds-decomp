
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
extern void _ZN9Animation7AdvanceEv(void* a);
extern int data_ov079_021275dc[];
extern void** data_ov079_021275ec[];

void func_ov079_021249f0(char* c)
{
    if ((unsigned int)(*(int*)(c + 0x3b4) - 0xa) <= 1) {
        *(unsigned char*)(c + 0x40c) = *(unsigned char*)(c + 0x40d);
        *(unsigned short*)(c + 0x100) = *(unsigned short*)(c + 0x3fe) + 1;
        *(int*)(c + 0x3b4) = 7;
    }
    if (*(unsigned char*)(c + 0x40c) == 0) {
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0x328) = 0x2000;
        if (*(unsigned short*)(c + 0x100) >= 0x20) {
            unsigned char* p = (unsigned char*)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
            (*p)++;
        } else {
            short *p94 = (short*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFFLL);
            *p94 = *p94 + 0x400;
            *(short*)(c + 0x8e) = *(short*)(c + 0x94);
        }
    } else {
        *(int*)(c + 0x98) = data_ov079_021275dc[*(unsigned char*)(c + 0x414)];
        if (*(unsigned short*)(c + 0x100) > 0x2a) {
            *(int*)(c + 0x3b0) = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                c + 0x2cc,
                data_ov079_021275ec[*(unsigned char*)(c + 0x414) * 5 + 3][1],
                0, 0x1000, 0);
        }
    }
    _ZN9Animation7AdvanceEv(c + 0x31c);
}
