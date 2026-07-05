typedef unsigned char u8;

extern void func_02011b38(void *c, int a1, int a2, int a3, short a5);
extern void func_02011698(char *table, char *n);

int func_0201186c(char *thiz, int a1, int a2, short a3)
{
    int i = 0;
    do {
        if (*(int *)(thiz + *(u8 *)(thiz + 4) * 0x14 + 8) == 0) {
            *(int *)thiz += 1;
            if (*(int *)thiz == 0) {
                *(int *)thiz += 1;
            }
            func_02011b38(thiz + 8 + *(u8 *)(thiz + 4) * 0x14, *(int *)thiz, a1, a2, a3);
            func_02011698(thiz, thiz + 8 + *(u8 *)(thiz + 4) * 0x14);
            return *(int *)thiz;
        }
        *(u8 *)(thiz + 4) = (*(u8 *)(thiz + 4) + 1) % 0x40;
    } while (++i < 0x40);
    return 0;
}
