typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 data_0209fcd8;
extern u8 data_0209fc88;
extern u8 data_0209fcc8;
extern u32 data_0209fd18;
extern u8* data_0209fd0c;
extern u8 data_0208f174[];
extern u8 data_0208f074[];
extern void func_02031028(void* a);

void func_02031cd4(void* arg)
{
    int i;
    u8* base;
    u8 cmd;
    u8* entry;
    u32 type;
    u16 v;

    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    i = 0;

    for (;;) {
        base = data_0209fd0c;
        cmd = base[i];
        switch (cmd) {
        case 0xff:
            data_0209fcc8 = 8;
            return;
        case 0xfe:
            entry = base + i;
            type = entry[2];
            data_0209fd18 = type;
            v = entry[4] | (entry[3] << 8);
            if (type == 1 && v == 1)
                func_02031028(arg);
            entry = data_0209fd0c + i;
            i += entry[1];
            break;
        case 0xfd:
            data_0209fcc8 = 2;
            return;
        default:
            if (arg == 0)
                data_0209fc88 += data_0208f074[cmd];
            else
                data_0209fc88 += data_0208f174[cmd];
            i += 1;
            data_0209fcd8 += 1;
            break;
        }
    }
}
