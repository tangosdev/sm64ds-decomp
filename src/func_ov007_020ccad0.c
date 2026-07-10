// Actor factory: new ActorBase(0x64), base ctor, installs the vtable chain
// (two base vtables then the ov007 one), ORs flag bits 1 and 4 at +0x13 via a
// materialized byte pointer, sets the +0x50 sub-object vtables, and constructs
// the member at +0x54 via func_02017278.
extern void *_ZN9ActorBasenwEj(unsigned int size);
extern void _ZN9ActorBaseC1Ev(void *p);
extern void func_02017278(int *p);

extern int data_0208e4b8[];
extern int data_02092680[];
extern int data_ov007_021032e8[];
extern int data_0208ee14[];
extern int data_ov007_021032b0[];

int *func_ov007_020ccad0(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x64);
    if (p) {
        unsigned char *f;
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)data_0208e4b8;
        p[0] = (int)data_02092680;
        f = (unsigned char *)((long long)(int)((char *)p + 0x13) & 0xFFFFFFFFFFFFFFFFLL);
        *f |= 1;
        *f |= 4;
        p[0] = (int)data_ov007_021032e8;
        p[0x50 / 4] = (int)data_0208ee14;
        p[0x50 / 4] = (int)data_ov007_021032b0;
        func_02017278(p + 0x54 / 4);
    }
    return p;
}
