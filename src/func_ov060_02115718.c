extern void func_ov060_02111cc0(void *c, int a, int b);
extern int Bowser_IsAnimAtLastFrame(void *c);
void func_ov060_02115718(void *c)
{
    func_ov060_02111cc0(c, 0x11, 0x0);
    *(int*)((char*)c + 0x98) = 12288;
    Bowser_IsAnimAtLastFrame(c);
}
