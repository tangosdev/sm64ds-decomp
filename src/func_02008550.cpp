//cpp
struct Obj { int v; };
typedef int (Obj::*PMF)(unsigned char *, int, int);

extern "C" {
extern int data_0209b004;
extern PMF KS_CAMERA_FUNCTIONS[39];
extern PMF data_02086c80, data_02086d80, data_02086c60, data_02086c90, data_02086bd8, data_02086d18, data_02086d98, data_02086c88, data_02086d00, data_02086bc0, data_02086e30, data_02086ce8, data_02086e38, data_02086e20, data_02086cd0, data_02086e08, data_02086df0, data_02086cb8, data_02086dd8, data_02086dc0, data_02086c20, data_02086da0, data_02086d60, data_02086d88, data_02086cf8, data_02086d68, data_02086c78, data_02086d40, data_02086d38, data_02086d20, data_02086c50, data_02086d08, data_02086cf0, data_02086c38, data_02086cd8, data_02086cc0, data_02086cb0, data_02086ca8, data_02086ca0;
}

extern "C" int func_02008550(Obj *obj, unsigned char *msg, int a2, int a3)
{
    if ((data_0209b004 & 1) == 0) {
        data_0209b004 |= 1;
        KS_CAMERA_FUNCTIONS[0] = data_02086c80;
        KS_CAMERA_FUNCTIONS[1] = data_02086d80;
        KS_CAMERA_FUNCTIONS[2] = data_02086c60;
        KS_CAMERA_FUNCTIONS[3] = data_02086c90;
        KS_CAMERA_FUNCTIONS[4] = data_02086bd8;
        KS_CAMERA_FUNCTIONS[5] = data_02086d18;
        KS_CAMERA_FUNCTIONS[6] = data_02086d98;
        KS_CAMERA_FUNCTIONS[7] = data_02086c88;
        KS_CAMERA_FUNCTIONS[8] = data_02086d00;
        KS_CAMERA_FUNCTIONS[9] = data_02086bc0;
        KS_CAMERA_FUNCTIONS[10] = data_02086e30;
        KS_CAMERA_FUNCTIONS[11] = data_02086ce8;
        KS_CAMERA_FUNCTIONS[12] = data_02086e38;
        KS_CAMERA_FUNCTIONS[13] = data_02086e20;
        KS_CAMERA_FUNCTIONS[14] = data_02086cd0;
        KS_CAMERA_FUNCTIONS[15] = data_02086e08;
        KS_CAMERA_FUNCTIONS[16] = data_02086df0;
        KS_CAMERA_FUNCTIONS[17] = data_02086cb8;
        KS_CAMERA_FUNCTIONS[18] = data_02086dd8;
        KS_CAMERA_FUNCTIONS[19] = data_02086dc0;
        KS_CAMERA_FUNCTIONS[20] = data_02086c20;
        KS_CAMERA_FUNCTIONS[21] = data_02086da0;
        KS_CAMERA_FUNCTIONS[22] = data_02086d60;
        KS_CAMERA_FUNCTIONS[23] = data_02086d88;
        KS_CAMERA_FUNCTIONS[24] = data_02086cf8;
        KS_CAMERA_FUNCTIONS[25] = data_02086d68;
        KS_CAMERA_FUNCTIONS[26] = data_02086c78;
        KS_CAMERA_FUNCTIONS[27] = data_02086d40;
        KS_CAMERA_FUNCTIONS[28] = data_02086d38;
        KS_CAMERA_FUNCTIONS[29] = data_02086d20;
        KS_CAMERA_FUNCTIONS[30] = data_02086c50;
        KS_CAMERA_FUNCTIONS[31] = data_02086d08;
        KS_CAMERA_FUNCTIONS[32] = data_02086cf0;
        KS_CAMERA_FUNCTIONS[33] = data_02086c38;
        KS_CAMERA_FUNCTIONS[34] = data_02086cd8;
        KS_CAMERA_FUNCTIONS[35] = data_02086cc0;
        KS_CAMERA_FUNCTIONS[36] = data_02086cb0;
        KS_CAMERA_FUNCTIONS[37] = data_02086ca8;
        KS_CAMERA_FUNCTIONS[38] = data_02086ca0;
    }
    return (obj->*KS_CAMERA_FUNCTIONS[msg[6]])(msg + 7, a2, a3);
}
