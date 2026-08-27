extern char data_ov018_02113c4c[];
extern void func_ov018_02112398(void *self);

void func_ov018_021123d0(char *self, int i) {
    *(char **)(self + 0x370) = data_ov018_02113c4c + (i << 4);
    func_ov018_02112398(self);
}
