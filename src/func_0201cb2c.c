extern int data_0209d6fc;
extern int data_0209d6e8;
extern int MSG_FILE_PTR;
extern int data_0209d6ec;
extern int MSG_ARR_PTR;

void func_0201cb2c(void) {
    int ip = data_0209d6fc;
    int r2 = ip + 0x20;
    data_0209d6e8 = ip;
    MSG_FILE_PTR = r2;
    int r3 = r2 + *(int*)(r2 + 4);
    data_0209d6ec = r3;
    MSG_ARR_PTR = ip + 0x30;
}
