typedef unsigned short u16;
typedef unsigned char u8;

extern u16 ReadUnalignedUshort(u8* p);
extern u16 data_0209b274;
extern u8 data_020a0e40;
extern u16 data_0209f49e;
extern u16 data_0209f49c;

#define LAUNDER(x) ((int)(((long long)(int)(x))))

int func_ov002_020bd3a0(char* p0, u8* p1, unsigned int p2)
{
    u16 val = ReadUnalignedUshort(p1);
    if (p2 == data_0209b274) {
        *(u16*)((char*)&data_0209f49e + LAUNDER(*(u8*)(p0 + 0x6d8)) * 0x18) =
            val | *(u16*)((char*)&data_0209f49e + LAUNDER(data_020a0e40) * 0x18);
    }
    val |= *(u16*)((char*)&data_0209f49c + LAUNDER(data_020a0e40) * 0x18);
    *(u16*)((char*)&data_0209f49c + LAUNDER(*(u8*)(p0 + 0x6d8)) * 0x18) = val;
    return 1;
}
