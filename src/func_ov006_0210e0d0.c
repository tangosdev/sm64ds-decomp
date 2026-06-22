struct V2 { int a, b; };
void func_ov006_0211474c(void *self, int x, int y, struct V2 *v);
extern void *data_ov006_0213ed24;
char *func_ov006_0210e0d0(char *self, int x, int y, struct V2 *src) {
    struct V2 v;
    v.a = src->a;
    v.b = src->b;
    func_ov006_0211474c(self, x, y, &v);
    *(void**)(self) = &data_ov006_0213ed24;
    *(int*)(self + 0x28) = 0x18000;
    *(short*)(self + 0x32) = 0;
    return self;
}
