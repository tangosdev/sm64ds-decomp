#pragma scheduling off
extern unsigned char unk_38070B8[];
extern int dword_38070B4;
extern int fn_02383450(int, int);

int fn_02383658(int channel, int volume, int pan, int timer, int timer2) {
    int base = channel << 4;
    int packed;
    char *io;
    unk_38070B8[channel] = (unsigned char)volume;
    if (dword_38070B4 > 0 && ((1 << channel) & 0xFFF5) != 0)
        volume = fn_02383450(volume, timer2);
    volume |= (timer2 << 16) | 0x60000000 | (pan << 8);
    packed = volume;
    io = (char *)(base + 0x04000000);
    *(int *)(io + 0x400) = packed;
    io += 0x400;
    *(unsigned short *)(io + 8) = (unsigned short)(0x10000 - timer);
    return (int)io;
}
