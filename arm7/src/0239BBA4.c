/* storage 0x0239BBA4 runtime 0x027CDB1C main ARM. */

extern unsigned short *fn_0239EF88(unsigned short *node);
extern void fn_0238477C(int a, int b);
extern void fn_0239F778(unsigned short *node);
extern void fn_0239554C(unsigned int key, unsigned int cb);
extern void fn_0239FA98(int a);
extern void fn_0239FB20(int a);
extern void fn_0239F7F8(int a, int b);
extern void fn_0239B7E8(void);

void fn_0239BBA4(void) {
    unsigned short *state;
    unsigned short status;

    state = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC);
    status = *state;
    switch (status) {
    case 0x50:
        goto hit50;
    case 0x51:
        goto done;
    case 0x53:
        goto hit53;
    default:
        goto done;
    }
hit50: {
        unsigned short *node;

        node = fn_0239EF88((unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x10));
        if (node == 0) {
            *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = 8;
            *state = 0x53;
            fn_0238477C(2, 3);
        } else {
            *state = 0x51;
            fn_0239F778(node);
            fn_0239554C(*(unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x18), 0x027CDAE0);
        }
        goto done;
    }
hit53:
    fn_0239FA98(1);
    fn_0239FB20(1);
    fn_0239F7F8(1, 0);
    *state = 0;
    fn_0239B7E8();
done:
    ;
}
