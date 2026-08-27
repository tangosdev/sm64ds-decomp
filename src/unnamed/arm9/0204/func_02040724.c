extern unsigned int data_020a0f94;
extern int data_020a0f5c;
extern int Wireless_Reset(void);

void func_02040724(void)
{
    switch (data_020a0f94) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        data_020a0f5c = 0;
        break;
    case 3:
    case 4:
        data_020a0f5c = 0;
        Wireless_Reset();
        break;
    }
}
