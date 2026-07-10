extern int _ZN6Player17SetNoControlStateEhih(void* p, unsigned char a, int b, unsigned char d);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_ov089_0213115c(char* c, int i);
extern void _ZN5Event6SetBitEj(unsigned int a);
extern int data_0209caa0[];
extern unsigned char data_0209f2ac;

void func_ov089_02131df4(char* c, char* player)
{
    if (data_0209caa0[1] & (2 << *(int*)(c + 0x444)))
        data_0209f2ac = 0;
    else
        data_0209f2ac = 1;
    data_0209caa0[1] |= (2 << *(int*)(c + 0x444));

    if (*(int*)(c + 0x444) <= 1) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    } else if (*(int*)(c + 0x444) != 7) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    }

    func_ov089_0213115c(c, 3);
    *(char**)(c + 0x110) = player;
    {
        char* pl = *(char**)(c + 0x110);
        /* force separate base materialization like ROM: r2=c+0xb0, r3=pl+0x5c */
        int* fl = (int*)(unsigned)((unsigned long long)(unsigned)(c + 0xb0));
        int* s = (int*)(unsigned)((unsigned long long)(unsigned)(pl + 0x5c));
        int t0 = s[0];
        int ev = 0x1d;
        *(int*)(c + 0x5c) = t0;
        *(int*)(c + 0x60) = s[1];
        *(int*)(c + 0x64) = s[2];
        {
            int ang = *(short*)(*(char**)(c + 0x110) + 0x8e);
            *(short*)(c + 0x8e) = ang;
        }
        *fl &= ~0x40000;
        _ZN5Event6SetBitEj(ev);
    }
}
