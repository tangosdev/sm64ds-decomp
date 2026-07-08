typedef unsigned int u32;

typedef struct Sub Sub;
struct Sub {
    char _pad[0x24];
    int count; /* 0x24 */
};

typedef struct Obj Obj;
struct Obj {
    Sub *sub;   /* 0x00 */
    char *data; /* 0x04 */
};

void func_02046120(Obj *self, int a)
{
    int i;
    int n = self->sub->count;

    for (i = 0; i < n; i++)
    {
        volatile u32 *p = (volatile u32 *)(((long long)(int)((int)self->data + i * 0x30 + 0x24)) & 0xFFFFFFFFFFFFFFFFLL);
        *p &= 0xc0ffff0f;
        *p |= 0x40;
        *p |= 0x30;
        *p &= ~0x1f0000;
        *p |= a << 16;
    }
}
