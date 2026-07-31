extern void _ZN9Animation7AdvanceEv(void* a);
extern void func_02012694(int a, void* p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
extern void* data_ov079_021281b0[];

void func_ov079_02124ed4(char* c)
{
    _ZN9Animation7AdvanceEv(c + 0x31c);
    *(unsigned char*)(c + 0x405) = 0;
    if (*(unsigned char*)(c + 0x40c) == 0) {
        if (*(unsigned char*)(c + 0x402) != 0) {
            *(unsigned char*)(c + 0x402) = 0;
            {
                unsigned char* p401 = (unsigned char*)(((int)c + 0x401));
                *p401 = *p401 - 1;
            }
            func_02012694(0x134, c + 0x74);
            {
                int* p = (int*)(((int)*(int*)(c + 0x3ac) + 0x5c));
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(3, p[0], p[1], p[2]);
            }
            if (*(unsigned char*)(c + 0x401) == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x2cc, data_ov079_021281b0[1], 0x40000000, 0x1000, 0);
                *(int*)(c + 0x3b0) = 8;
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x2cc, data_ov079_021281b0[1], 0x40000000, 0x1000, 0);
                *(int*)(c + 0x3ac) = 0;
                _ZN9Animation7AdvanceEv(c + 0x330);
                {
                    unsigned char* p40c = (unsigned char*)(((int)c + 0x40c));
                    *p40c = *p40c + 1;
                }
            }
        }
        *(unsigned short*)(c + 0x300 + 0xfc) = 0;
        return;
    } else {
        unsigned short v;
        {
            unsigned short* p3fc = (unsigned short*)(((int)c + 0x3fc));
            *p3fc = *p3fc + 1;
        }
        v = *(unsigned short*)(c + 0x300 + 0xfc);
        if (v <= 10) {
            if ((int)v % 2) {
                int* p60 = (int*)(((int)c + 0x60));
                *p60 = *p60 + 0x10000;
            } else {
                int* p60 = (int*)(((int)c + 0x60));
                *p60 = *p60 - 0x10000;
            }
            return;
        }
        *(unsigned char*)(c + 0x40c) = 10;
        *(unsigned short*)(c + 0x300 + 0xfc) = 0;
    }
}
