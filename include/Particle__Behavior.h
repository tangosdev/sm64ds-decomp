#ifndef PARTICLE__BEHAVIOR_H
#define PARTICLE__BEHAVIOR_H

#include "Particle__Element.h"

namespace Particle {

/* Particle behavior records are read directly from the particle resource.
 * func_0204a17c establishes the size of each record. Field names below are
 * inferred from the matching behavior functions. */
struct AccelerationData {
    s16 x;
    s16 y;
    s16 z;
    u16 pad_006;
};

struct JitterData {
    s16 xAmplitude;
    s16 yAmplitude;
    s16 zAmplitude;
    u16 period;
};

struct ConvergeData {
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    s16 strength;
    u16 pad_00e;
};

struct TurnData {
    u16 angle;
    u16 axis;
};

struct LimitPlaneData {
    s32 position;
    s16 restitution;
    u16 mode : 2;
    u16 pad_006 : 14;
};

struct RadiusConvergeData {
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    s16 strength;
    u16 pad_00e;
};

union EffectData {
    AccelerationData acceleration;
    JitterData jitter;
    ConvergeData converge;
    TurnData turn;
    LimitPlaneData limitPlane;
    RadiusConvergeData radiusConverge;
    u8 raw[0x10];
};

struct Acceleration {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

struct Jitter {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

struct Converge {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

struct Turn {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

struct LimitPlane {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

struct RadiusConverge {
    static void Func(EffectData& effect, char* particle, Vector3& velocity);
};

typedef char AccelerationData_size_must_be_0x8[
    sizeof(AccelerationData) == 0x8 ? 1 : -1];
typedef char JitterData_size_must_be_0x8[
    sizeof(JitterData) == 0x8 ? 1 : -1];
typedef char ConvergeData_size_must_be_0x10[
    sizeof(ConvergeData) == 0x10 ? 1 : -1];
typedef char TurnData_size_must_be_0x4[
    sizeof(TurnData) == 0x4 ? 1 : -1];
typedef char LimitPlaneData_size_must_be_0x8[
    sizeof(LimitPlaneData) == 0x8 ? 1 : -1];
typedef char RadiusConvergeData_size_must_be_0x10[
    sizeof(RadiusConvergeData) == 0x10 ? 1 : -1];
typedef char EffectData_size_must_be_0x10[
    sizeof(EffectData) == 0x10 ? 1 : -1];
} // namespace Particle

#endif
