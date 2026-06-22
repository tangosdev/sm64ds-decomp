typedef unsigned short u16;
extern int func_0203fcec(void);
extern int func_0203fcb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

void func_020405b4(u16 *p)
{
    if (p[1] == 0) {
        switch (data_020a0f5c) {
        case 0:
            func_0203fcec();
            return;
        case 3:
        case 4:
            func_0203fcb0();
            return;
        }
        return;
    }
    data_020a0f94 = 1;
}
