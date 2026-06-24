extern short data_02082214[];
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);

void func_ov060_02112d48(char *c, int arg)
{
    char *o = (char *)_ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x3ac));
    if (o == 0)
        return;
    short angle = *(short *)(c + 0x408);
    unsigned int idx = (unsigned short)(short)(angle + 0x8000) >> 4;
    *(short *)(o + 0x31e) = (short)((arg * data_02082214[idx * 2 + 1]) >> 12);
    *(short *)(o + 0x322) = (short)((arg * -data_02082214[idx * 2]) >> 12);
}
