extern void func_ov060_02111cc0(void *c, int a, int b);
extern int func_ov060_02115a30(void *c);
void func_ov060_02113710(void *c)
{
    func_ov060_02111cc0(c, 0xf, 0);
    int r = func_ov060_02115a30(c);
    if (r != 0) {
        *(int*)((char*)c + 0x40c) = 0;
    }
}
