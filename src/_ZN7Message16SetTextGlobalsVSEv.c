extern int VS_MSG_FILE_PTR;
extern int data_0209fd04;
extern int data_0209fcf8;
extern int data_0209fcfc;
extern int data_0209fd08;

void _ZN7Message16SetTextGlobalsVSEv(void) {
    int ip = VS_MSG_FILE_PTR;
    int r2 = ip + 0x20;
    data_0209fd04 = ip;
    data_0209fcf8 = r2;
    int r3 = r2 + *(int*)(r2 + 4);
    data_0209fcfc = r3;
    data_0209fd08 = ip + 0x30;
}
