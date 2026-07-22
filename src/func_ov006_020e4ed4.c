typedef unsigned char u8;
typedef unsigned short u16;

extern void func_02012718(void *a, int b);

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

void func_ov006_020e4ed4(char *this)
{
    int idx;
    int flag = 0;
    int x;
    int y;

    idx = data_020a0e40;
    if (data_020a0de8[idx * (4 & 0xFFFFFFFF)] != 0)
    {
        if (data_020a0de9[idx * 4] != 0)
            flag = 1;
    }
    if (flag == 0)
        return;

    x = ((*((int *)(this + 0x5584))) >> 0xc) -
        data_020a0dea[idx * 4];
    y = ((*((int *)(this + 0x5588))) >> 0xc) -
        data_020a0deb[idx * 4];

    *((int *)(this + 0x5594)) = x << 0xc;
    *((int *)(this + 0x5598)) = y << 0xc;
    *((u8 *)(this + 0x55b8)) = 1;
    *((u16 *)(this + 0x55b2)) = 0xc000;

    if (*((u8 *)(this + 0x55bd)) == 0)
    {
        func_02012718((void *)0x1d2, *((int *)(this + 0x5584)));
        *((u8 *)(this + 0x55bd)) = 6;
    }

    *((int *)(this + 0x55a0)) = 0;
    *((int *)(this + 0x55a4)) = 0;
    *((int *)(this + 0x558c)) =
        (*((int *)(this + 0x5584))) + (*((int *)(this + 0x5594)));
    *((int *)(this + 0x5590)) =
        (*((int *)(this + 0x5588))) + (*((int *)(this + 0x5598)));
    *((int *)(this + 0x55a8)) = 0xff;
    *((u8 *)(this + 0x55be)) = 0;
}
