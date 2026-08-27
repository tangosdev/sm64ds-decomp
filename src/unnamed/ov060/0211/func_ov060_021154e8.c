extern int func_ov060_02111f08(void *c);
extern int func_ov060_02111cc0(void *c, int a, int b);
extern int func_ov060_02115518(void *c);
void func_ov060_021154e8(void *c) {
    func_ov060_02111f08(c);
    *(int*)((char*)c + 0x98) = 0;
    func_ov060_02111cc0(c, 0x10, 0);
    func_ov060_02115518(c);
}
