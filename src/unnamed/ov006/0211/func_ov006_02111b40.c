struct V2 { int a, b; };
void func_ov006_0211474c(void *self, int x, int y, struct V2 *v);
extern void *_ZTV21cMgSmartball_kinoko_c;
char *func_ov006_02111b40(char *self, int x, int y, struct V2 *src) {
    struct V2 v;
    v.a = src->a;
    v.b = src->b;
    func_ov006_0211474c(self, x, y, &v);
    *(void**)(self) = &_ZTV21cMgSmartball_kinoko_c;
    *(int*)(self + 0x28) = 0x7000;
    *(int*)(self + 0x34) = 2;
    return self;
}
