struct E {
    int a;
    int b;
    unsigned char tag;
    char pad[3];
};

extern struct E data_020a7f60[];

unsigned char func_0205b4d0(int idx, int a, int b)
{
    struct E *e;

    e = &data_020a7f60[idx];
    e->a = a;
    e->b = b;
    e->tag++;
    return e->tag;
}