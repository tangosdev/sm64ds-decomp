typedef unsigned char u8;

extern void func_02011af8(void *c, int a1, int a2, int a3, int s0, short s1);
extern void func_02011698(char *table, char *n);

int func_0201179c(char *thiz, int a1, int a2, int a3, short s1)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(int *)(thiz + *(u8 *)(thiz + 4) * 0x14 + 8) == 0) {
            *(int *)thiz += 1;
            if (*(int *)thiz == 0) *(int *)thiz += 1;
            func_02011af8(thiz + 8 + *(u8 *)(thiz + 4) * 0x14, *(int *)thiz, a1, a2, a3, s1);
            func_02011698(thiz, thiz + 8 + *(u8 *)(thiz + 4) * 0x14);
            return *(int *)thiz;
        }
        *(u8 *)(thiz + 4) = (*(u8 *)(thiz + 4) + 1) % 0x40;
    }
    return 0;
}
