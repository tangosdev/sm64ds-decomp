extern unsigned short data_0209d6d4;
extern unsigned char data_0209d660;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d668;
extern int func_0201eaac();
extern int func_0201e220(int);

void _ZN7Message19DisplaySaveMenuTextEt(unsigned short n) {
    data_0209d6d4 = n;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d668 = 1;
    func_0201e220(0);
}
