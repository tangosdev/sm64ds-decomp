extern int _ZN9Animation8FinishedEv(void *anim);
extern void func_ov085_0212bc78(void *c, void *p);
extern int data_ov085_0213067c[];

int func_ov085_0212b86c(void *c)
{
    if (_ZN9Animation8FinishedEv((char *)c + 0x350) != 0)
        func_ov085_0212bc78(c, data_ov085_0213067c);
    return 1;
}
