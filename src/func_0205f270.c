typedef unsigned short u16;

extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void func_0205ba64(int a, void *fn);
extern void func_0205f300(void);

extern char data_020a80c8[];
extern char data_020a80cc[];

void func_0205f270(void)
{
    if (*(u16 *)data_020a80c8 != 0) return;
    *(u16 *)data_020a80c8 = 1;
    func_0205b858();
    *(u16 *)(data_020a80cc + 0x32) = 0;
    *(u16 *)(data_020a80cc + 0x36) = 0;
    *(u16 *)(data_020a80cc + 0xc) = 0;
    *(int *)(data_020a80cc + 0) = 0;
    *(int *)(data_020a80cc + 0x10) = 0;
    *(u16 *)(data_020a80cc + 0x30) = 0;
    *(u16 *)(data_020a80cc + 0x34) = 0;
    while (func_0205ba3c(6, 1) == 0)
        ;
    func_0205ba64(6, (void *)func_0205f300);
}
