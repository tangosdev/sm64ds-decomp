typedef void (*CopyFn)(void *self, void *buf, int len);

struct Stream {
    char pad[0x10];
    CopyFn fn;
};

extern unsigned char data_02086880[];

#pragma optimize_for_size on
void func_0206c9f4(struct Stream *self, unsigned int n)
{
    while (n != 0) {
        int chunk = 0x10;
        if (n < 0x10) chunk = n;
        self->fn(self, data_02086880, chunk);
        n -= chunk;
    }
}
