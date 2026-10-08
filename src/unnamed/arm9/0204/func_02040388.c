extern int func_0203fcec(void);
extern int func_0203fcb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

void func_02040388(unsigned short *p)
{
    int s;
    if (p[1] == 0) {
        s = data_020a0f5c;
        switch (s) {
        case 0:
            func_0203fcec();
            break;
        case 3:
        case 4:
            func_0203fcb0();
            break;
        }
        return;
    }
    data_020a0f94 = 1;
}
