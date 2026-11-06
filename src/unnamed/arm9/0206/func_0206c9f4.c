typedef void (*CopyFn)(void *self, void *buf, int len);

struct Stream {
    char pad[0x10];
    CopyFn fn;
};

extern const unsigned char data_02086880[20];

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
