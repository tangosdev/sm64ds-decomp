extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
extern void func_02012694(int a, void* p);

extern int data_ov084_02130ce8[2];
extern int data_ov084_02130cf0[2];

typedef struct {
    unsigned char b0 : 1;
    unsigned char flag : 1;
} Flags;

void func_ov084_0212934c(char* c)
{
    unsigned int kind;
    int v;
    int type;
    unsigned char *fp;

    if (*(int*)(c + 0x434) != 0)
        return;

    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1b4))
        return;

    /* kind before type load: forces v=r3 / type1=r1 coloring (short extract form) */
    v = *(int*)(c + 0x3c8);
    kind = (unsigned short)((unsigned)v >> 12);
    type = *(int*)(c + 0x3d0);

    if (type == data_ov084_02130ce8[1]) {
        if (kind <= 4 || (kind >= 0xc && kind <= 0x10)) {
            if (((Flags*)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, c + 0x74);
            fp = (unsigned char *)(((int)c + 0x468));
            *fp |= 2;
            return;
        }
        fp = (unsigned char *)(((int)c + 0x468));
        *fp &= ~2;
        return;
    }

    if (type == data_ov084_02130cf0[1]) {
        if (kind <= 3 || (kind >= 0x10 && kind <= 0x13)) {
            if (((Flags*)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, c + 0x74);
            fp = (unsigned char *)(((int)c + 0x468));
            *fp |= 2;
            return;
        }
        fp = (unsigned char *)(((int)c + 0x468));
        *fp &= ~2;
    }
}
