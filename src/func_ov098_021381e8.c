extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void func_ov098_02138ce0(void *);
extern void func_ov098_02139850(void *);
extern void func_ov098_021397c8(void *);
void func_ov098_021381e8(char *c)
{
    if (_ZN4dBgW9IsEnabledEv(c + 0x124))
        _ZN4dBgW7DisableEv(c + 0x124);
    func_ov098_02138ce0(c);
    func_ov098_02139850(c);
    func_ov098_021397c8(c);
    (*(int *)(((int)c + 0xb0))) &= ~0xe0000;
}
