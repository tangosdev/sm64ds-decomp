struct Obj {
    void *vt;          /* 0x0 */
    char pad[2];       /* 0x4 */
    short f6;          /* 0x6 */
    short f8;          /* 0x8 */
};

extern void _ZN5dPa_c7level_c16simpleCallback_cC2Ev(char *p);
extern int data_0208f414;

struct Obj *func_020225fc(struct Obj *thiz)
{
    _ZN5dPa_c7level_c16simpleCallback_cC2Ev((char *)thiz);
    thiz->vt = &data_0208f414;
    thiz->f6 = 0x1000;
    thiz->f8 = 0;
    return thiz;
}
