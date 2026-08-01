int func_ov055_02111288(char *dst, char *src)
{
    int *vec = (int *)(((int)src + 0x5c));
    int x = vec[0];
    int z = vec[2];
    int y = vec[1];

    *(int *)(dst + 0x5c) = -x;
    *(int *)(dst + 0x60) = y;
    *(int *)(dst + 0x64) = z;
    *(short *)(dst + 0x8e) = -*(short *)(src + 0x8e);
    return 1;
}
