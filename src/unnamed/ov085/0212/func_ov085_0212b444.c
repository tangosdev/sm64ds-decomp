extern int _ZN9Animation8FinishedEv(void *);
extern int func_ov085_0212bc78(void *c, void *p);
extern int data_ov085_021306cc[];

int func_ov085_0212b444(char *c)
{
    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        func_ov085_0212bc78(c, data_ov085_021306cc);
    }
    return 1;
}
