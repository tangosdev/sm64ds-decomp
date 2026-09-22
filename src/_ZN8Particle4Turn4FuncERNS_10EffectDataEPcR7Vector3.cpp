//cpp
#include "Particle__Behavior.h"
#include "math/Matrix.h"

extern "C" {
extern s16 data_02082214[];
void func_02052550(Matrix3x3* matrix, int sin, int cos);
void func_0205256c(Matrix3x3* matrix, int sin, int cos);
void Matrix3x3_SetRotationZ(Matrix3x3* matrix, int sin, int cos);
void MulVec3Mat3x3(const Vector3* in, const Matrix3x3* matrix, Vector3* out);
}

void Particle::Turn::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;
    Matrix3x3 matrix;
    int idx;

    switch (effect.turn.axis) {
    case 0:
        idx = effect.turn.angle >> 4;
        func_02052550(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 1:
        idx = effect.turn.angle >> 4;
        func_0205256c(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 2:
        idx = effect.turn.angle >> 4;
        Matrix3x3_SetRotationZ(&matrix, data_02082214[idx * 2],
                              data_02082214[idx * 2 + 1]);
        break;
    }

    MulVec3Mat3x3(&state.offset, &matrix, &state.offset);
}
