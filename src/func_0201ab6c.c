typedef unsigned char u8;
typedef unsigned short u16;

extern void _ZN7Message7AddCharEc(int c);

struct S { u8 b[8]; };
extern struct S *CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6b4;
extern u8 data_0209d688;
extern u8 data_0209d68c;
extern u8 data_0209d6b0;
extern u8 data_0209d6e4[];
extern u8 data_0209d694;

void func_0201ab6c(void) {
    struct S *p = CURR_MSG_TEXT_CHAR;
    u16 v = p->b[4] | (p->b[3] << 8);
    u8 idx;
    switch (v) {
    case 0:
        data_0209d688 = data_0209d6b4;
        break;
    case 1:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b0;
        _ZN7Message7AddCharEc(0x4d);
        data_0209d694 = 1;
        break;
    case 2:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b4;
        _ZN7Message7AddCharEc(0x4d);
        data_0209d694 = 2;
        break;
    case 3:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b4;
        _ZN7Message7AddCharEc(0x4d);
        data_0209d694 = 3;
        break;
    }
}
