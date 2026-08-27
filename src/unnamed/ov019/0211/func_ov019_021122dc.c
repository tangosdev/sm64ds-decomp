extern char data_ov019_0211356c[];
extern void func_ov019_021122a4(void *self);

void func_ov019_021122dc(char *self, int i) {
    *(char **)(self + 0x370) = data_ov019_0211356c + (i << 4);
    func_ov019_021122a4(self);
}
