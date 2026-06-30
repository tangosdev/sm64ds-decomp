typedef unsigned char u8;
typedef unsigned short u16;

extern u8 NumStars(void);
extern int func_020138dc(void);
extern int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern int __aeabi_idiv(int, int);
extern void _ZN7Message7AddCharEc(int c);

struct MsgTextChar {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 indexHi;  /* 3 */
    u8 indexLo;  /* 4 */
    u8 mode;     /* 5 */
    u8 base;     /* 6 */
};

extern struct MsgTextChar* data_0209d6f4;
extern u8 data_0208ee74[];
extern u16 data_0208ee64[];

void func_0201aca4(void) {
    struct MsgTextChar* state = data_0209d6f4;
    u16 idx = state->indexLo | (state->indexHi << 8);
    switch (idx) {
        case 0: {
            u8 mode = state->mode;
            u8 num = 0;
            int leading = 0;
            u8 base = state->base;
            int i;
            switch (mode) {
                case 0: num = base - NumStars(); break;
                case 1: num = func_020138dc(); break;
                case 2: num = base - _ZN8SaveData22NumGlowingRabbitsFoundEv(); break;
            }
            for (i = 0; i < 3; i++) {
                int q = __aeabi_idiv(num, data_0208ee64[i]);
                if (q != 0 || leading != 0 || i == 2) {
                    _ZN7Message7AddCharEc(data_0208ee74[q]);
                    leading = 1;
                }
                num = num % data_0208ee64[i];
            }
            break;
        }
        case 1:
            break;
    }
}
