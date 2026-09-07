extern int Vram__Map(int arg, unsigned short *p);

int func_02054430(int arg)
{
    unsigned short *p = (unsigned short *)0x020a6088;
    *p |= arg;
    return Vram__Map(arg, p);
}
