/* storage 0x02385CCC runtime 0x037FDB64 size 0xA8 wram. */

extern int fn_02385E78(int stream);
extern int fn_02385E20(int stream);
extern int fn_02385D74(int stream);
extern int fn_02384BD8(int key, int index);
extern int fn_02383828(void);

int fn_02385CCC(int stream, int key, int kind) {
    int result;
    int low;
    int high;
    int span;
    int rnd;
    short *row;

    switch (kind) {
    case 0:
        result = fn_02385E78(stream);
        break;
    case 1:
        result = fn_02385E20(stream);
        break;
    case 2:
        result = fn_02385D74(stream);
        break;
    case 4:
        low = fn_02385E78(stream);
        row = (short *)fn_02384BD8(key, low);
        if (row != 0)
            result = *row;
        break;
    case 3:
        low = fn_02385E20(stream) << 16;
        high = (short)fn_02385E20(stream);
        rnd = fn_02383828();
        span = (high - (low >> 16) + 1) * rnd;
        high = span >> 16;
        result = high + (low >> 16);
        break;
    default:
        break;
    }
    return result;
}
