extern void func_ov060_02111cc0(void *c, int a, int b);
extern int Bowser_IsAnimAtLastFrame(void *c);
int func_ov060_02115744(void *c)
{
    func_ov060_02111cc0(c, 0x13, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame(c);
    if (r != 0) {
        *(int*)((char*)c + 0x98) = 0x3000;
        return 1;
    }
    return 0;
}
