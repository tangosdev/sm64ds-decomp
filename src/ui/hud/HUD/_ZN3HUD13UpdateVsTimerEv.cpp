//cpp
extern "C" {
extern unsigned char data_0209f2bc;
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern unsigned short data_ov002_02111188;
extern int data_0208ee44;
extern unsigned char data_0209f204;
extern unsigned char data_0209d454;
void func_02012790(int);
}

struct HUD {
    void UpdateVsTimer();
};

void HUD::UpdateVsTimer()
{
    unsigned short *q;
    unsigned int v;

    if (data_0209f2bc != 0)
        return;
    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) != 0)
        return;

    if (data_ov002_02111188 != 0)
        data_ov002_02111188 = data_ov002_02111188 - data_0208ee44;

    if (data_0209f204 != 0)
        return;

    if (data_ov002_02111188 == 0)
    {
        data_ov002_02111188 = 0xb4;
        q = (unsigned short *)((int)this + 0x60);
        *q = *q - 1;
        v = *(unsigned short *)((char *)this + 0x60);
        if (v <= 3 && v != 0)
            func_02012790(0x28);
        if (*(unsigned short *)((char *)this + 0x60) == 0)
        {
            data_0209f204 = 1;
            data_ov002_02111188 = 0x3c;
            func_02012790(0x29);
            data_0209d454 &= ~4;
        }
    }

    if (*(unsigned short *)((char *)this + 0x60) > 3)
        return;
    if (*(unsigned short *)((char *)this + 0x66) < 0x30)
    {
        q = (unsigned short *)((int)this + 0x66);
        *q = *q + 4;
    }
    else
    {
        *(unsigned short *)((char *)this + 0x66) = 0x38;
    }
}
