/* storage 0x0239F064 runtime 0x027D0FDC size 0xA4 main. */

extern char *fn_02394E7C(char *list, unsigned int size);
extern void fn_02394FC4(unsigned int code);
extern void fn_02382CC8(char *src, char *dst, unsigned int count);
extern void fn_023969D8(char *dst, unsigned short *src);

unsigned short *fn_0239F064(unsigned short *arg, unsigned short value) {
    char *block;
    unsigned short *obj;

    block = fn_02394E7C(*(char **)0x0380FFF4 + 0x14C, 0x36);
    if (block == 0) {
        fn_02394FC4(2);
        return (unsigned short *)block;
    }
    *(unsigned short *)(block + 0xC) = 0xFFFF;
    obj = (unsigned short *)(block + 0x10);
    fn_02382CC8(*(char **)0x0380FFF4 + 0x64C, (char *)obj, 0x2C);
    obj[0x16] = value;
    obj[3] = 2;
    obj[9] = (unsigned short)(obj[3] + 0x1C);
    obj[10] = 0xA0;
    fn_023969D8((char *)obj + 0x18, arg);
    return obj;
}
