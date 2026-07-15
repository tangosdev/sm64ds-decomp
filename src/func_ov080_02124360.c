extern int RandomIntInternal(int *seed);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int data_0209e650;

void *func_ov080_02124360(char *c)
{
    unsigned char n = *(unsigned char *)(c + 0x180);
    void *obj;
    if (n == 0)
        return (void *)(unsigned int)n;
    *(unsigned char *)(c + 0x181) = 0;
    for (;;) {
        unsigned int r = ((unsigned int)RandomIntInternal(&data_0209e650)) >> 8;
        unsigned int cnt = *(unsigned char *)(c + 0x183);
        unsigned int idx = r % cnt;
        unsigned int id = *(unsigned int *)(c + 0x16c + (idx * 4));
        obj = _ZN5Actor10FindWithIDEj(id);
        if (obj == 0)
            continue;
        if (*(unsigned char *)((char *)obj + 0x181) != 0)
            continue;
        *(unsigned char *)((char *)obj + 0x181) = 1;
        return obj;
    }
}
