extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned);
extern int RandomIntInternal(int* seed);
extern void *data_ov066_0211ae2c[];
extern void *data_ov066_0211ae9c[];
extern void *data_ov066_0211ae3c[];
extern void *data_ov066_0211aebc[];
extern int data_0209e650;

void func_ov066_02116390(void* thiz)
{
    char* c = (char*)thiz;
    DecIfAbove0_Short((unsigned short*)(c + 0x66c));
    if (*(unsigned short*)(c + 0x66c) != 0)
        return;
    if (*(unsigned char*)(c + 0x66e) == 0) {
        if (*(int*)(c + 0x49c) == 2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae2c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae9c[1], 0x40000000, 0x1000, 0);
        *(unsigned short*)(c + 0x66c) = (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf) * 2 + 0x32;
    } else {
        if (*(int*)(c + 0x49c) == 2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
        *(unsigned short*)(c + 0x66c) = 8;
    }
    *(unsigned char*)(((int)c + 0x66e) & 0xFFFFFFFFFFFFFFFF) ^= 1;
}
