void func_ov006_02106a08(char *p, int idx)
{
    unsigned short *c = (unsigned short *)(int)(((long long)(int)(p + 0x4e30)) & 0xFFFFFFFFFFFFFFFFLL);
    unsigned char *q;
    c[idx] = c[idx] + 1;
    if ((((int)*(unsigned short *)(p + idx * 2 + 0x4e30) >> 3) & 1) != 0)
    {
        q = (unsigned char *)(int)(((long long)(int)(p + 0x4f66)) & 0xFFFFFFFFFFFFFFFFLL);
        q[idx] = 1;
    }
    else
    {
        q = (unsigned char *)(int)(((long long)(int)(p + 0x4f66)) & 0xFFFFFFFFFFFFFFFFLL);
        q[idx] = 0;
    }
    {
        int idxL = (int)(((long long)idx) & 0xFFFFFFFFFFFFFFFFLL);
        char *b = p + idxL * 2;
        b += 0x4e00;
        if (((unsigned short *)b)[0x18] < 0x40)
            return;
        ((unsigned short *)b)[0x18] = 0;
        *(unsigned char *)(p + idx + 0x4efa) = 0;
        q[idx] = 1;
    }
}
