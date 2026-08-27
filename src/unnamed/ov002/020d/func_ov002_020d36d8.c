extern int func_ov002_020c0434(char* c);
extern void func_ov002_020c0364(char* c, unsigned int arg);
extern int func_ov002_020dd824(char* c);
extern int func_ov002_020d5c6c(char* c);
extern int func_ov002_020dde74(char* c);
extern int func_ov002_020d674c(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int func_ov002_020d1204(char* c);

extern unsigned char data_020a0e40;
extern unsigned char data_0209f49e[];
extern unsigned char data_0209f49c[];
extern int data_ov002_0211004c;
extern int data_ov002_021104e4;
extern int data_ov002_0211019c;
extern int data_ov002_021101b4;

int func_ov002_020d36d8(char* c, int arg)
{
    if ((unsigned short)(*(unsigned short*)(c + 0x6ce) & 0x40)) return 0;

    if (func_ov002_020c0434(c) != 0) {
        func_ov002_020c0364(c, 3);
        return 1;
    }
    if (func_ov002_020dd824(c) != 0) return 1;
    if (func_ov002_020d5c6c(c) != 0) return 1;

    {
        int idx = data_020a0e40 * 0x18;
        unsigned short f49e = *(unsigned short*)(data_0209f49e + idx);
        if (f49e & 1) {
            return func_ov002_020dde74(c) != 0;
        }
        if ((*(unsigned short*)(data_0209f49c + idx) & 0x400) && (arg & 1) == 0) {
            if (func_ov002_020d674c(c) != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211004c);
            } else {
                *(unsigned char*)(c + 0x6e3) = 0;
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021104e4);
            }
            return 1;
        }
        if (f49e & 2) {
            if (*(unsigned short*)(c + 0x6b0) == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211019c);
                return 1;
            }
            if (func_ov002_020d1204(c) != 0) return 1;
        }
    }

    if (*(unsigned char*)(c + 0x6de) != 0) {
        if (*(int*)(c + 0x60) - *(int*)(c + 0x644) >= 0x28000) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
            return 1;
        }
    }
    return 0;
}
