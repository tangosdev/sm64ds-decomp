typedef unsigned char u8;

extern u8* CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6a4;
extern u8 data_0209d6b0;
extern u8 data_0208f174[];
extern u8 TALK_FONT_CHAR_WIDTHS[];
extern u8 data_0209d6bc;

void func_0201b6f8(int mode)
{
    int i;
    u8* p = CURR_MSG_TEXT_CHAR;
    data_0209d6a4 = 0;
    data_0209d6b0 = 0;
    i = 0;
    for (;;) {
        u8 v = p[i];
        i++;
        switch (v) {
        case 0xff:
            data_0209d6bc = 8;
            return;
        case 0xfd:
            data_0209d6bc = 2;
            return;
        case 0xfe:
            break;
        default:
            if (mode == 0)
                data_0209d6b0 = data_0209d6b0 + TALK_FONT_CHAR_WIDTHS[v];
            else
                data_0209d6b0 = data_0209d6b0 + data_0208f174[v];
            data_0209d6a4 = data_0209d6a4 + 1;
            break;
        }
    }
}
