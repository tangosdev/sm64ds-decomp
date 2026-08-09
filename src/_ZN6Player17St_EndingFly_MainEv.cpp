extern int func_ov002_020c3bdc(char* c);
extern int func_ov002_020c3a48(char* c);
extern int func_ov002_020c38a0(char* c);

int func_ov002_020c3d1c(char* c)
{
    switch (*(unsigned char*)(c+0x6e6)) {
    case 0:
        func_ov002_020c3bdc(c);
        break;
    case 1:
        if (func_ov002_020c3a48(c) == 0) break;
    case 2:
        func_ov002_020c38a0(c);
        break;
    }
    return 1;
}
