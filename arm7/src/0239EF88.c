/* storage 0x0239EF88 runtime 0x027D0F00 size 0xDC main. */

extern char *fn_02394E7C(char *list, unsigned int size);
extern void fn_02394FC4(unsigned int code);
extern void fn_02382CC8(char *src, char *dst, unsigned int count);
extern unsigned int fn_0239E83C(char *p);
extern unsigned int fn_0239E784(char *p);
extern void fn_023969D8(char *dst, unsigned short *src);

unsigned short *fn_0239EF88(unsigned short *arg) {
    char **pp;
    char *block;
    unsigned short *obj;
    unsigned int head;
    unsigned int tail;

    block = fn_02394E7C(*(char **)0x0380FFF4 + 0x14C, 0x5E);
    if (block == 0) {
        fn_02394FC4(2);
        return (unsigned short *)block;
    }
    *(unsigned short *)(block + 0xC) = 0xFFFF;
    obj = (unsigned short *)(block + 0x10);
    fn_02382CC8(*(char **)0x0380FFF4 + 0x64C, (char *)obj, 0x2C);
    pp = (char **)0x0380FFF4;
    obj[0x16] = *(unsigned short *)(*pp + 0x300 + 0x78);
    obj[0x17] = *(unsigned short *)(*pp + 0x300 + 0x6C);
    head = fn_0239E83C((char *)obj + 0x30);
    tail = fn_0239E784((char *)obj + 0x30 + head);
    obj[3] = (unsigned short)(head + tail + 4);
    obj[9] = (unsigned short)(obj[3] + 0x1C);
    obj[10] = 0;
    fn_023969D8((char *)obj + 0x18, arg);
    return obj;
}
