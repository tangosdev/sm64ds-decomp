void func_ov004_020b51e4(char* p);

extern char data_ov004_020bfa34[];   /* array, stride 0x24 */
extern int  data_ov004_020bc7d4;
extern void* data_ov004_020bf9f8;
extern int  data_ov004_020bfa04;
extern short data_ov004_020bf9e4;
extern void* data_ov004_020bf9e8;
extern int  data_ov004_020bfa24;
extern void* data_ov004_020bfa20;
extern void func_ov004_020b556c(void);

void func_ov004_020b56c8(char* p)
{
    int i;
    int cond;
    char* it = data_ov004_020bfa34;
    for (i = 0; i < 0x14; i++) {
        func_ov004_020b51e4(it);
        it += 0x24;
    }
    cond = data_ov004_020bc7d4;
    data_ov004_020bf9f8 = p;
    data_ov004_020bfa04 = 0;
    data_ov004_020bf9e4 = 3;
    if (cond != 0) {
        data_ov004_020bf9e8 = p;
        data_ov004_020bfa24 = 1;
    }
    data_ov004_020bfa20 = (void*)func_ov004_020b556c;
}
