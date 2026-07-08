extern unsigned short CURR_MSG_ID;
extern unsigned char PLAYER_TALKING;
extern unsigned char MSG_LINE_HEIGHT;
extern unsigned char data_0209d668;
extern int func_0201eaac();
extern int func_0201e220(int);

void _ZN7Message19DisplaySaveMenuTextEt(unsigned short n) {
    CURR_MSG_ID = n;
    PLAYER_TALKING = 0;
    func_0201eaac();
    MSG_LINE_HEIGHT = 0;
    data_0209d668 = 1;
    func_0201e220(0);
}
