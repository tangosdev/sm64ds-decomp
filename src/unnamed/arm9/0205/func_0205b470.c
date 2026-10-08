struct E {
    void (*fn)(int);
    int arg;
    unsigned char tag;
};

extern struct E data_020a7f60[];

void func_0205b470(int arg)
{
    struct E *e = &data_020a7f60[arg & 0xff];
    if (((arg >> 8) & 0xff) != e->tag)
        return;
    if (e->fn == 0)
        return;
    e->fn(e->arg);
}
