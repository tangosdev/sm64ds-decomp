void func_0204fa2c(int* p, int a);

extern int  data_ov007_02104bb4;
extern int  data_ov007_02104bb0;
extern int* data_ov007_02104bbc;
extern int* data_ov007_02104bb8;
extern int  data_ov007_02104bac;

void func_ov007_020bdebc(void)
{
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        j = i + 0x35;
        if (data_ov007_02104bb8[i] == 0) {
            if (data_ov007_02104bbc[j] != 0) {
                func_0204fa2c(&data_ov007_02104bbc[j], 1);
                if (i == 0x35) {
                    data_ov007_02104bb4 = 0;
                    data_ov007_02104bb0 = 0;
                    data_ov007_02104bac = 0;
                }
            }
        }
        data_ov007_02104bb8[i] = 0;
    }
}
