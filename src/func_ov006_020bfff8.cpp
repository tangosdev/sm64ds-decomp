//cpp
namespace cstd {
int fdiv(int numerator, int denominator);
}

extern "C" {
void MulVec3Mat4x3(void* v, void* m, void* out);
extern short data_02082214[];

void func_ov006_020bfff8(char* camera, void* worldPosition, int* screenX, int* screenY) {
    int viewPosition[3];
    MulVec3Mat4x3(worldPosition, camera, viewPosition);
    int angleIndex = *(short*)(camera + 0xb8) >> 4;
    int tableIndex = angleIndex * 2;
    // The shared table stores sine before cosine.
    int sine = data_02082214[tableIndex];
    int cosine = data_02082214[tableIndex + 1];
    int tangent = cstd::fdiv(sine, cosine);
    int halfHeight = (int)(((long long)tangent * (long long)(-viewPosition[2]) + 0x800) >> 0xc);
    int normalizedY = cstd::fdiv(viewPosition[1], halfHeight);
    int scaledY = (int)(((long long)normalizedY * (long long)0x5f800u + 0x800) >> 0xc);
    screenY[0] = -((scaledY + (int)0xfffa0800) >> 0xc);
    int halfWidth = (int)(((long long)halfHeight * (long long)0x1555u + 0x800) >> 0xc);
    int normalizedX = cstd::fdiv(viewPosition[0], halfWidth);
    int scaledX = (int)(((long long)normalizedX * (long long)0x7f800u + 0x800) >> 0xc);
    screenX[0] = (scaledX + 0x7f800) >> 0xc;
}
}
