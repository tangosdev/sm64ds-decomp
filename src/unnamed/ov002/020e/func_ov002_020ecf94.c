extern int func_ov002_020ec654(unsigned char *p);
extern void func_ov002_020ecb0c(unsigned char *p);
extern void func_ov002_020ecd18(unsigned char *p);

void func_ov002_020ecf94(unsigned char *p)
{
    if (func_ov002_020ec654(p))
        func_ov002_020ecb0c(p);
    else
        func_ov002_020ecd18(p);
}
