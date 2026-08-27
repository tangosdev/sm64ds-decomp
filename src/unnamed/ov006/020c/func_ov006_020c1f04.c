void func_ov006_020c1f04(char* c, int* src) {
    if (src != 0) {
        *(int*)(c+0x80) = src[0];
        *(int*)(c+0x84) = src[1];
        *(int*)(c+0x88) = src[2];
    } else {
        *(int*)(c+0x80) = 0;
        *(int*)(c+0x84) = 0x2000;
        *(int*)(c+0x88) = -0x2000;
    }
    *(int*)(c+0xa8) = 1;
}
