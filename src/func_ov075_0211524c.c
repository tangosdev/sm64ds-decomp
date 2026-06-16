typedef unsigned char u8;
extern u8 data_0209fc50[];
extern u8 data_0209fc64[];
extern int data_ov075_0211b594[];
int func_ov075_0211524c(void* unused, int idx) {
    unsigned int cnt = data_0209fc50[0];
    int r2;
    if (cnt <= 1) {
        r2 = 0;
    } else {
        r2 = cnt - 1;
        idx = data_0209fc64[idx];
        if (idx < 0) idx = 3;
    }
    return *(int*)((char*)data_ov075_0211b594 + r2*16 + idx*4);
}
