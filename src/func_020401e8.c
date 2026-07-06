extern int func_0203fbfc(void);
extern int func_0203fb98(void);

extern int data_020a0f5c;

void func_020401e8(unsigned short *p)
{
    if (p[1] == 0) {
        switch (data_020a0f5c) {
        case 0:
            func_0203fbfc();
            break;
        case 3:
            if (func_0203fb98() != 0) {
                return;
            }
            data_020a0f5c = 0;
            func_0203fbfc();
            break;
        case 4:
            data_020a0f5c = 4;
            func_0203fbfc();
            break;
        }
    } else {
        data_020a0f5c = 0;
        func_0203fbfc();
    }
}
