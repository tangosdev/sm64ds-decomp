typedef void (*CopyFn)(void *self, void *buf, int len);

struct Stream {
    char pad[0x10];
    CopyFn fn;
};

extern unsigned char data_02086880[];

/* optimize_for_size is this library's own build setting, not a matching lever:
   the ISD debug console and its soft-float helpers were shipped size-optimized. */
#pragma optimize_for_size on
/* Write n spaces through the stream's output callback (+0x10), up to 16 at
   a time, from the sixteen spaces at data_02086880. func_0206a928 pads
   fields to width with it. */
void func_0206c9f4(struct Stream *self, unsigned int n)
{
    while (n != 0) {
        int chunk = 0x10;
        if (n < 0x10) chunk = n;
        self->fn(self, data_02086880, chunk);
        n -= chunk;
    }
}
