extern void func_ov060_02111cc0(void *c, int a, int b);
extern int Bowser_IsAnimAtLastFrame(void *c);
void func_ov060_021156ec(void *c)
{
    func_ov060_02111cc0(c, 0x12, 0x40000000);
    *(int*)((char*)c + 0x98) = 0;
    Bowser_IsAnimAtLastFrame(c);
}
