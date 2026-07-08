struct E7f60 {
    void (*fn)(int);
    int arg;
    unsigned char tag;
};

extern struct E7f60 data_020a7f60[];

unsigned char func_0205b4d0(int index, void (*fn)(int), int arg) {
    struct E7f60 *e = &data_020a7f60[index];
    unsigned char *t = (unsigned char *)(((long long)(int)((char *)e + 8)) & 0xFFFFFFFFFFFFFFFFLL);
    data_020a7f60[index].fn = fn;
    e->arg = arg;
    (*t)++;
    return e->tag;
}
