extern void func_ov060_02111cc0(void *c, int a, int b);
extern int Bowser_IsAnimAtLastFrame(void *c);

void func_ov060_02113710(void *c) {
    func_ov060_02111cc0(c, 0xf, 0);
    if (Bowser_IsAnimAtLastFrame(c) != 0) {
        *(int *)((char *)c + 0x40c) = 0;
    }
}
