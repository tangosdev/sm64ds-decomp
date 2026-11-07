#pragma opt_propagation off
extern unsigned char unk_3808544[];
extern void fn_0238219C(void *);
extern void fn_0238244C(void *);
extern int fn_02386834(void);
extern void fn_023822A8(void *, int, int, void *, int);
extern long long fn_02381EB0(void);
extern void fn_0238222C(void *, long long, long long, void *, int);

int fn_023868A8(int a1) {
    long long v56;
    long long field;
    unsigned char *e;
    long long now;
    int id;
    e = &unk_3808544[a1 << 6];
    if (unk_3808544[a1 << 6] != 0) {
        fn_0238219C(e + 0x14);
        e[0] = 0;
    }
    v56 = *(long long *)(e + 4);
    field = *(long long *)(e + 0xC);
    id = a1 | (e[1] << 8);
    fn_0238244C(e + 0x14);
    if (field == 0)
        fn_023822A8(e + 0x14, (int)v56, (int)(v56 >> 32), fn_02386834, id);
    else {
        now = fn_02381EB0();
        fn_0238222C(e + 0x14, v56 + now, field, fn_02386834, id);
    }
    e[0] = 1;
    return 1;
}
