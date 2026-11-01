/* storage 0x0239BDBC runtime 0x027CDD34 main ARM. */

extern void fn_02395DA8(void);
extern void fn_0239554C(unsigned int key, unsigned int cb);
extern void fn_02395CB8(void);
extern void fn_0239B7E8(void);

void fn_0239BDBC(void) {
    unsigned short *state;
    unsigned short status;

    state = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC);
    status = *state;
    switch (status) {
    case 0x20:
        goto hit20;
    case 0x21:
        goto done;
    case 0x25:
        goto hit25;
    default:
        goto done;
    }
hit20:
    fn_02395DA8();
    state[2] = 0;
    state[3] = 0;
    *state = 0x21;
    fn_0239554C(*(unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x10), 0x027CDCF8);
    goto done;
hit25:
    *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = state[2];
    *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 6) = state[3];
    if (state[2] != 0)
        fn_02395CB8();
    *state = 0;
    fn_0239B7E8();
done:
    ;
}
