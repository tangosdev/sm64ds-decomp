extern char *data_ov007_0210342c;

int func_ov007_020aebac(void)
{
    char *q = *(char **)(*(char **)(data_ov007_0210342c + 0x28));
    unsigned char *b = (unsigned char *)(((long long)(int)(q + 0xb)) & 0xFFFFFFFFFFFFFFFFLL);
    if (b[0] == 0 && b[1] == 0 && b[2] == 0 && *(int *)(q + 4) == 0)
        return 1;
    return 0;
}
