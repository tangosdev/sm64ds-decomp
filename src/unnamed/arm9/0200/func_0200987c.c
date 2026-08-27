extern int DecIfAbove0_Short(void *p);
extern void func_0200cb58(void *obj, int index);
extern void func_02009e70(void *obj);

void func_0200987c(void *self) {
    if (DecIfAbove0_Short((char *)self + 0x196) == 0) {
        func_0200cb58(self, 0x14);
    }
    func_02009e70(self);
}
