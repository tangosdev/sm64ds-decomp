typedef struct
{
    unsigned char pad[0x14];
    unsigned int lenHi;
    unsigned int lenLo;
    int count;
    unsigned char buf[0x40];
} HashCtx;

extern void func_0205e3d4(HashCtx* c);

#define CNT (*(int*)(int)((char*)c + 0x1c))

void func_0205e280(HashCtx* c)
{
    if (c->count > 0x37)
    {
        int n = c->count;
        CNT++;
        c->buf[n] = 0x80;
        while (c->count < 0x40)
        {
            c->buf[c->count++] = 0;
        }
        func_0205e3d4(c);
        while (c->count < 0x38)
        {
            c->buf[c->count++] = 0;
        }
    }
    else
    {
        int n = c->count;
        CNT++;
        c->buf[n] = 0x80;
        while (c->count < 0x38)
        {
            c->buf[c->count++] = 0;
        }
    }

    c->buf[0x38] = c->lenLo >> 24;
    c->buf[0x39] = c->lenLo >> 16;
    c->buf[0x3a] = c->lenLo >> 8;
    c->buf[0x3b] = c->lenLo;
    c->buf[0x3c] = c->lenHi >> 24;
    c->buf[0x3d] = c->lenHi >> 16;
    c->buf[0x3e] = c->lenHi >> 8;
    c->buf[0x3f] = c->lenHi;

    func_0205e3d4(c);
}
