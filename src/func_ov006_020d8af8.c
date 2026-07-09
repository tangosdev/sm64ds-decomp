typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_ov006_020d836c(char* c);
extern int func_ov006_020d8c88(char* c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int i);

void func_ov006_020d8af8(char* self)
{
    int found;
    int j;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;

    func_ov006_020d836c(self);

    if (*(u16*)(self + 0x62e8) != 0) {
        (*(u16*)(((long long)(int)(self + 0x62e8)) & 0xFFFFFFFFFFFFFFFFLL))--;
        if (*(s16*)(self + 0x62e8) > 0)
            return;
        *(u16*)(self + 0x62e8) = 0;
        *(u16*)(self + 0x62e0) = 0x10;
        if (func_ov006_020d8c88(self) != 0) {
            *(u8*)(self + 0x62f4) = 1;
            _ZN5Sound12PlayBank2_2DEj(0x1dd);
        }
        return;
    }

    if (*(u16*)(self + 0x62e0) != 0) {
        (*(u16*)(((long long)(int)(self + 0x62e0)) & 0xFFFFFFFFFFFFFFFFLL))--;
        return;
    }

    found = 0;
    j = 0;
    do {
        if (arr[j][0x4698] == 1 && arr[j][0x4697] == 6 && arr[j][0x469b] == 4)
            found = j + 1;
        j++;
    } while (j < 0x70);

    (*(u16*)(((long long)(int)(self + 0x62ea)) & 0xFFFFFFFFFFFFFFFFLL))++;
    if (*(u16*)(self + 0x62ea) < 4)
        return;

    if (found != 0) {
        arr[found - 1][0x4698] = 2;
        *(u16*)(self + 0x62ea) = 0;
        (*(u16*)(((long long)(int)(self + 0x62ee)) & 0xFFFFFFFFFFFFFFFFLL))++;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
    } else {
        *(u16*)(self + 0x62ea) = 0;
        *(u16*)(self + 0x62e8) = 0x40;
        *(int*)(self + 0x62d4) = 3;
    }
}
