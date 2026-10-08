void func_ov007_020c934c(char *o, int v)
{
    short d = (short)(v - 1);
    *(short*)(o + 0) = (short)v;
    *(short*)(o + 8) = d;
    *(int*)(o + 4) = 0;
    *(short*)(o + 0xa) = d;
    *(int*)(o + 0xc) = 0;
    *(short*)(o + 2) = -1;
}
