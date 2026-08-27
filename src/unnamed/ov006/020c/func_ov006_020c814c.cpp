//cpp
extern "C" {
extern int data_ov006_0214041c;
extern int data_ov006_02140434;
int _ZNK9Animation12WillHitFrameEi(void *thiz, int frame);
void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c8658(void *c);
void func_ov006_020c814c(char *c)
{
    if (*(int *)(c + 0xac) == data_ov006_0214041c)
    {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x9c, 0xc) ||
            _ZNK9Animation12WillHitFrameEi(c + 0x9c, 0x18))
            func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x14));
    }
    if (*(int *)(c + 0x18) <= 0x180000)
        return;
    *(int *)(c + 0x24) = 0;
    data_ov006_02140434++;
    func_ov006_020c8658(c);
}
}
