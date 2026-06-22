extern int func_0203fd28(void);
extern int func_0203fbfc(void);
extern int data_020a0f94;
extern int data_020a0f5c;

void func_02040790(void)
{
    switch (data_020a0f94) {
    case 0:
        data_020a0f94 = 2;
        data_020a0f5c = 4;
        func_0203fd28();
        break;
    case 2:
        data_020a0f5c = 4;
        break;
    case 3:
        data_020a0f5c = 4;
        func_0203fbfc();
        break;
    case 1:
    case 4:
        break;
    }
}
