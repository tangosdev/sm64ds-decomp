typedef unsigned char u8;
typedef unsigned short u16;

extern u16 CURR_MSG_ID;
extern u8 PLAYER_TALKING;
extern u8 data_0209d668;
extern u8 MSG_LINE_HEIGHT;

extern void func_0201eaac(void);
extern void func_0201d418(int a, int b);

void func_0201cfb8(int arg0)
{
    CURR_MSG_ID = (u16)arg0;
    PLAYER_TALKING = 0;
    func_0201eaac();
    data_0209d668 = 1;
    MSG_LINE_HEIGHT = 0;
    func_0201d418(0xcf, 0x68);
}
