extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int i, int fix, unsigned int j);
extern void _ZN7PathPtr6FromIDEj(void *p, unsigned int id);
struct G { void *a; void *b; };
extern struct G data_ov030_02115d18;
int func_ov030_02112a14(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, data_ov030_02115d18.b, 0, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    _ZN7PathPtr6FromIDEj(c+0x398, *(int*)(c+8) & 0xff);
    *(int*)(c+0x3a0) = 1;
    *(char*)(c+0x3c7) = 0;
    *(int*)(c+0x98) = 0x6000;
    *(int*)(c+0x3b4) = 8;
    return 1;
}
