typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_0206a3a4(int arg0, int *out);
extern void func_0206a458(int *out);
extern void func_0206a424(int *out);
extern void func_0206a37c(int a, int *p);

extern u16 data_020a9de0[];

struct Card {
    u8 pad[5];
    u8 b0 : 1;
    u8 b1 : 1;
    u8 b2to7 : 6;
    u8 pad6[2];
    int w8;
};

int func_0206a4a0(void)
{
    int a[2];
    int b[2];
    int ret = 1;
    struct Card *card = (struct Card *)0x27ffc30;
    u8 *io = (u8 *)0x8000000;

    if (*(u16 *)card == 0xffff)
        return 0;
    if (card->b1 == 1)
        return 0;

    func_0206a3a4((u16)data_020a9de0[1], a);
    func_0206a458(b);

    if (io[0xb2] == 0x96) {
        if (*(u16 *)card != *(u16 *)(io + 0xbe))
            goto setflag;
    }
    if (io[0xb2] != 0x96) {
        if (*(u16 *)card != *(u16 *)0x801fffe)
            goto setflag;
    }
    if (card->w8 != *(int *)(io + 0xac) && card->b0) {
    setflag:
        {
            u8 *p = (u8 *)(((int)card + 5) & 0xFFFFFFFFFFFFFFFF);
            ret = 0;
            *p |= 2;
        }
    }

    func_0206a424(b);
    func_0206a37c((u16)data_020a9de0[1], a);
    return ret;
}
