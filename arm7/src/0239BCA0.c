/* storage 0x0239BCA0 runtime 0x027CDC18 main ARM. */

extern unsigned short *fn_0239E9C0(unsigned short *node, int zero);
extern void fn_0238477C(int a, int b);
extern void fn_0239F778(unsigned short *node);
extern void fn_0239554C(unsigned int key, unsigned int cb);
extern void fn_0239FA98(int a);
extern void fn_0239FB20(int a);
extern void fn_0239F7F8(int a, int b);
extern void fn_0239B7E8(void);

void fn_0239BCA0(void) {
    unsigned short *state;
    unsigned short status;

    state = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC);
    status = *state;
    switch (status) {
    case 0x30:
        goto hit30;
    case 0x31:
        goto done;
    case 0x35:
        goto hit35;
    default:
        goto done;
    }
hit30: {
        unsigned short *node;

        node = fn_0239E9C0((unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x10), 0);
        if (node == 0) {
            *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = 8;
            *state = 0x35;
            fn_0238477C(2, 2);
        } else {
            node[22] = *(unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x16);
            node[23] = 1;
            node[24] = 0;
            *state = 0x31;
            fn_0239F778(node);
            fn_0239554C(*(unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x18), 0x027CDBDC);
        }
        goto done;
    }
hit35:
    fn_0239FA98(1);
    fn_0239FB20(1);
    fn_0239F7F8(1, 0);
    *state = 0;
    fn_0239B7E8();
done:
    ;
}
