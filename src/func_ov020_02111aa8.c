extern int func_0201267c(int a, void *b);
extern void _Z14ApproachLinearRiii(int *p, int target, int step);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *clsn);

void func_ov020_02111aa8(char *c)
{
    *(unsigned char *)(c + 0x108) = 0;
    (*(unsigned short *)((int)c + 0x100))++;
    if (*(unsigned short *)((int)c + 0x100) >= 3) {
        int *bf;
        int t;
        int sid;
        *(int *)(c + 0x424) = 3;
        *(int *)(c + 0x98) = 0x32000;
        bf = (int *)(c + 0x234);
        t = *bf;
        sid = 0x166;
        *bf = t & ~1;
        func_0201267c(sid, c + 0x74);
    }
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x32000, 0x1000);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
}
