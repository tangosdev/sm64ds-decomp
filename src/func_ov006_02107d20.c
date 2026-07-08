void func_ov006_02107d20(int *p, int val)
{
    int i;
    *(short *)(((long long)(int)((char *)p + 0x16)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    for (i = 0; i < 5; i++) {
        if (p[i] == 0) {
            p[i] = val;
            return;
        }
    }
}
