//cpp
extern "C" {
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int);
extern void func_02011d20(void);
extern void _ZN7Message7EndTalkEv(void);
extern void func_ov074_021203e4(void *c, int i);
extern char *data_0209f318;
void func_ov074_0212018c(char *c)
{
    if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x5c8)) != -1)
        return;
    (*(int *)(((int)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFF)) &= ~8;
    _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
    func_02011d20();
    _ZN7Message7EndTalkEv();
    func_ov074_021203e4(c, 2);
}
}
