extern char* data_02103360;
extern char* data_0210342c;
extern void func_ov007_020b3d30(int a);

void func_ov007_020b3c54(int a, int mode) {
    short st = **(short**)data_02103360;
    switch (st) {
        case 0:
        case 1:
        case 2:
            mode = 0;
            break;
        case 7:
            mode = 1;
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            mode = 2;
            *(int*)(data_0210342c + 0x180) = st;
            break;
        case 14:
        case 15:
        case 16:
            mode = 3;
            break;
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            mode = 4;
            break;
        case 22:
        case 23:
            mode = 5;
            break;
        default:
            break;
    }
    *(int*)(data_02103360 + 8) = mode;
    func_ov007_020b3d30(*(int*)(data_02103360 + 4));
}
