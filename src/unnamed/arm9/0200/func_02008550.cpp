//cpp
struct Obj { int v; };
typedef int (Obj::*PMF)(unsigned char *, int, int);

extern "C" {
extern int data_0209b004;
extern PMF data_0209b138[39];
extern PMF data_02086c80, data_02086d80, data_02086c60, data_02086c90, data_02086bd8, data_02086d18, data_02086d98, data_02086c88, data_02086d00, data_02086bc0, data_02086e30, data_02086ce8, data_02086e38, data_02086e20, data_02086cd0, data_02086e08, data_02086df0, data_02086cb8, data_02086dd8, data_02086dc0, data_02086c20, data_02086da0, data_02086d60, data_02086d88, data_02086cf8, data_02086d68, data_02086c78, data_02086d40, data_02086d38, data_02086d20, data_02086c50, data_02086d08, data_02086cf0, data_02086c38, data_02086cd8, data_02086cc0, data_02086cb0, data_02086ca8, data_02086ca0;
}

extern "C" int func_02008550(Obj *obj, unsigned char *msg, int a2, int a3)
{
    if ((data_0209b004 & 1) == 0) {
        data_0209b004 |= 1;
        data_0209b138[0] = data_02086c80;
        data_0209b138[1] = data_02086d80;
        data_0209b138[2] = data_02086c60;
        data_0209b138[3] = data_02086c90;
        data_0209b138[4] = data_02086bd8;
        data_0209b138[5] = data_02086d18;
        data_0209b138[6] = data_02086d98;
        data_0209b138[7] = data_02086c88;
        data_0209b138[8] = data_02086d00;
        data_0209b138[9] = data_02086bc0;
        data_0209b138[10] = data_02086e30;
        data_0209b138[11] = data_02086ce8;
        data_0209b138[12] = data_02086e38;
        data_0209b138[13] = data_02086e20;
        data_0209b138[14] = data_02086cd0;
        data_0209b138[15] = data_02086e08;
        data_0209b138[16] = data_02086df0;
        data_0209b138[17] = data_02086cb8;
        data_0209b138[18] = data_02086dd8;
        data_0209b138[19] = data_02086dc0;
        data_0209b138[20] = data_02086c20;
        data_0209b138[21] = data_02086da0;
        data_0209b138[22] = data_02086d60;
        data_0209b138[23] = data_02086d88;
        data_0209b138[24] = data_02086cf8;
        data_0209b138[25] = data_02086d68;
        data_0209b138[26] = data_02086c78;
        data_0209b138[27] = data_02086d40;
        data_0209b138[28] = data_02086d38;
        data_0209b138[29] = data_02086d20;
        data_0209b138[30] = data_02086c50;
        data_0209b138[31] = data_02086d08;
        data_0209b138[32] = data_02086cf0;
        data_0209b138[33] = data_02086c38;
        data_0209b138[34] = data_02086cd8;
        data_0209b138[35] = data_02086cc0;
        data_0209b138[36] = data_02086cb0;
        data_0209b138[37] = data_02086ca8;
        data_0209b138[38] = data_02086ca0;
    }
    return (obj->*data_0209b138[msg[6]])(msg + 7, a2, a3);
}
