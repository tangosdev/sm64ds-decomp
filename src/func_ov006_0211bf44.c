typedef unsigned char u8;
typedef unsigned short u16;

extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int soundId);

void func_ov006_0211bf44(char* base, int slot)
{
    u8 player;
    int offset;
    int active;
    char* entry;
    int dx;
    int dy;

    if (*(int*)(base + 0x5608) != 1) {
        return;
    }
    if (*(u8*)(base + 0x5624) >= 2) {
        return;
    }

    player = data_020a0e40[0];
    offset = player * 4;
    active = 0;
    if (data_020a0de8[offset] != 0) {
        if (data_020a0de9[offset] != 0) {
            active = 1;
        }
    }
    if (active == 0) {
        return;
    }

    entry = base + slot * 0x14;
    entry += 0x5000;
    dx = data_020a0dea[player * 4] - (*(int*)(entry + 0xe8) >> 12);
    dy = data_020a0deb[player * 4] - (*(int*)(entry + 0xec) >> 12);

    if (dx > 0x18) {
        return;
    }
    if (dx < -0x18) {
        return;
    }
    if (dy > 0x18) {
        return;
    }
    if (dy < -0x18) {
        return;
    }

    *(u8*)(entry + 0xf4) = 1;
    *(u16*)(entry + 0xf0) = 0;
    *(u8*)(entry + 0xf7) = 0;
    {
        u8* counter = (u8*)(int)(((long long)(int)(base + 0x5624)));
        *counter = *counter + 1;
    }
    _ZN5Sound12PlayBank2_2DEj(0x201);
}
