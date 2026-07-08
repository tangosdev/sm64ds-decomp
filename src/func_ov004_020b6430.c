typedef unsigned char u8;

extern int func_ov004_020ad674(void);
extern void func_ov004_020af68c(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b653c(int arg);
extern void func_ov004_020b52b8(char *c);

extern int data_ov004_020bf9ec;
extern int data_ov004_020bfa18;
extern int FRAME_TIMER;
extern int *data_ov004_020bc828[];
extern int data_ov004_020bfa24;
extern int data_ov004_020bf9e8;
extern char data_ov004_020bfa34[];

void func_ov004_020b6430(void) {
    int i;
    char *p;
    if (data_ov004_020bf9ec == 0 && data_ov004_020bfa18 != 0) {
        if (FRAME_TIMER & 0x10) {
            func_ov004_020af68c((void*)data_ov004_020bc828[func_ov004_020ad674()][0], 0x14, 0x30, -1, -1);
        } else {
            func_ov004_020af68c((void*)data_ov004_020bc828[func_ov004_020ad674()][1], 0x14, 0x30, -1, -1);
        }
        func_ov004_020af68c((void*)data_ov004_020bc828[func_ov004_020ad674()][2], 0x14, 0x30, -1, -1);
    }
    if (data_ov004_020bfa24 != 0) {
        func_ov004_020b653c(data_ov004_020bf9e8);
    }
    for (p = data_ov004_020bfa34, i = 0; i < 0x14; i++) {
        func_ov004_020b52b8(p);
        p += 0x24;
    }
}
