extern int Vram__Map(int arg, unsigned short *p);
extern unsigned short data_020a6088;

int func_02054430(int arg)
{
    unsigned short *p = &data_020a6088;
    *p |= arg;
    return Vram__Map(arg, p);
}
