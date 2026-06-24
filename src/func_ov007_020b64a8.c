extern int func_ov007_020b7eec(int x);
struct S {
    char pad28[0x28];
    int **p28;     /* 0x28 */
    void (*fn2c)(int); /* 0x2c */
};
extern struct S *data_ov007_0210342c;
void func_ov007_020b64a8(int arg0)
{
    int idx = *(unsigned char*)((char*)data_ov007_0210342c->p28[0] + 8) + arg0;
    if (idx < 0) idx = 2;
    else if (idx > 2) idx = 0;
    switch (idx) {
    case 0: data_ov007_0210342c->fn2c(1); break;
    case 1: data_ov007_0210342c->fn2c(2); break;
    case 2: data_ov007_0210342c->fn2c(3); break;
    }
    func_ov007_020b7eec((int)data_ov007_0210342c->p28[0]);
}
