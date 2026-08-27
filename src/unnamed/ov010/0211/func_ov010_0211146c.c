// @symbol func_ov010_0211146c
/* recovered: shared common types */
#include "common.h"
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern short data_02082214[];

extern struct Matrix4x3 data_020a0e68;
void func_ov010_0211146c(char *c) {
    int a = (int)(*(unsigned short*)(c+0x8e)) >> 4;
    int a2 = a << 1;
    int b = (int)(*(unsigned short*)(c+0x90)) >> 4;
    int cosB = data_02082214[b << 1];
    int sinA = data_02082214[a2 + 1];
    int cosA = data_02082214[a2];
    int r5 = cosB * 5;
    int dx = (int)(((long long)r5 * sinA + 0x800) >> 12);
    int dz = (int)(((long long)r5 * cosA + 0x800) >> 12);
    int X = (*(int*)(c+0x5c) - dx) >> 3;
    int Y = *(int*)(c+0x60) >> 3;
    int Z = (*(int*)(c+0x64) + dz) >> 3;
    Matrix4x3_FromTranslation(&data_020a0e68, X, Y, Z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0x8e));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(c+0x90));
    *(struct Matrix4x3*)(c+0x33c) = data_020a0e68;
}
