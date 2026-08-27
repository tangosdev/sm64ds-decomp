typedef int Fix12i;

extern short ReadUnalignedShort(const void *p);
extern void func_020070e8(void *self, Fix12i a, int b, int c, int d, int e, int f);

struct Obj
{
    char pad0[0x19a];
    short unk19a;       /* 0x100 + 0x9a */
};

struct Data
{
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;          /* +2 */
    unsigned char b3;
    unsigned char b4;
    unsigned char b5;          /* +5 */
    unsigned char b6;
    unsigned char b7;
    unsigned char b8;          /* +8 */
};

int func_02007a8c(struct Obj *self, struct Data *data)
{
    Fix12i a = ReadUnalignedShort(data) << 12;
    int b = data->b2 << 4;
    int c = ReadUnalignedShort(&data->b3);
    int d = data->b5;
    int e = ReadUnalignedShort(&data->b6);
    if (e == -1)
        e = self->unk19a;
    func_020070e8(self, a, b, c, d, e, data->b8);
    return 1;
}
