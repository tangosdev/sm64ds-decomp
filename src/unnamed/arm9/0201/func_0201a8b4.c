extern long long func_02059650(void);
extern int func_0201aa50(void *a, unsigned int lo, unsigned int hi, void *fn, void *e);
extern unsigned short func_0203da80(unsigned short x);
extern void func_02013f28(void);

typedef struct {
    char pad[0x30];
    unsigned long long limit;   /* 0x30 */
    unsigned long long base;    /* 0x38 */
} Obj;

void func_0201a8b4(void *thiz)
{
    Obj *o = (Obj *)thiz;
    unsigned long long delta = (unsigned long long)func_02059650() - o->base;

    if (delta < o->limit) {
        unsigned long long rem = o->limit - delta;
        if ((unsigned int)((rem << 6) / 0x82ea) != 0)
            func_0201aa50(thiz, (unsigned int)rem, (unsigned int)(rem >> 32),
                          (void *)func_0201a8b4, thiz);
    } else {
        if (func_0203da80(0x2000) == 0)
            func_02013f28();
    }
}
