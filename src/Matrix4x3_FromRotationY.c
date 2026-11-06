#include "common.h"

extern void func_02052820(Matrix4x3 *m, int s, int c);
extern short data_02082214[];

void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052820(m, data_02082214[a*2], data_02082214[a*2+1]);
}
