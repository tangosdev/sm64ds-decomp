extern void func_ov060_02111cc0(void *c, int a, int b);
extern int Bowser_IsAnimAtLastFrame(void *c);
void func_ov060_021142b4(void *c)
{
    *(int*)((char*)c + 0x98) = 0;
    unsigned short* p = (unsigned short*)((char*)c + 0x3fc);
    if (*p == 0) {
        *(unsigned short*)((char*)c + 0x3fe) = 0;
    }
    func_ov060_02111cc0(c, 0x1b, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame(c);
    if (r != 0) {
        *(int*)((char*)c + 0x40c) = 0xb;
    }
}
