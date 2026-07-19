extern int func_0202044c(char *thiz, int idx);
extern void func_0202043c(char *thiz);

extern int data_0208ee44;

void func_02020768(char *thiz)
{
    int *acc;
    unsigned char *cnt;

    if (*(unsigned char *)(thiz + 0x25) == 0)
        return;

    acc = (int *)(((int)(thiz + 0x1c)) & 0xFFFFFFFFFFFFFFFF);
    *acc += data_0208ee44;

    if (*(int *)(thiz + 0x1c) < *(int *)(thiz + 0x18))
        return;

    *(int *)(thiz + 0x1c) = 0;

    if (func_0202044c(thiz, *(int *)(thiz + 0x14) + 1) != 0)
        return;

    if (*(unsigned char *)(thiz + 0x24) == 0) {
        func_0202043c(thiz);
        return;
    }

    if (*(unsigned char *)(thiz + 0x24) <= 1) {
        *(unsigned char *)(thiz + 0x25) = 0;
        return;
    }

    cnt = (unsigned char *)(((int)(thiz + 0x24)) & 0xFFFFFFFFFFFFFFFF);
    *cnt -= 1;
    func_0202043c(thiz);
}
