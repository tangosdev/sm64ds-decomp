#include "types.h"
/* Three-stop RGB555 colour ramp entry (SPL's SPLColorAnim). */
struct ColorRamp
{
    u16 startColor;
    u16 endColor;
    u8 stop1;
    u8 stop2;
    u8 stop3;
    u8 pad7;
    u16 flags;
};

/* The particle definition's header (func_0204a730's PtclData; SPL's
 * SPLResourceHeader). 0x12 is the particle's base colour: the ramp runs
 * startColor -> color -> endColor, and func_0204c584 spawns a particle with
 * it (its `tex` there; the ramp is its `texAnim` view). */
struct PtclData {
    char pad0[0x12];
    u16 color; /* 0x12 */
};

/* The particle definition (func_0204a730's PtclDef; SPL's SPLResource), whose
 * colorAnim slot registers this function as a particle callback. */
struct PtclDef {
    struct PtclData *data;       /* 0x00 */
    void *scaleAnim;             /* 0x04 */
    struct ColorRamp *colorAnim; /* 0x08 */
};

struct Particle
{
    char pad0[0x3a];
    u16 color;
};

/* age is the particle's animation time, 0..255 (func_0204a730 passes its
 * life rate, or its loop time when the ramp loops). */
void func_0204d294(struct Particle *ptcl, struct PtclDef *data, int age) {
    struct PtclData *header = data->data;
    struct ColorRamp *ramp = data->colorAnim;
    u8 stop1 = ramp->stop1;
    u8 stop2 = ramp->stop2;
    u8 stop3 = ramp->stop3;

    if (age < stop1)
    {
        ptcl->color = ramp->startColor;
        return;
    }
    if (age < stop2)
    {
        u16 hiColor = header->color;
        u16 loColor = ramp->startColor;
        u16 flags = ramp->flags;
        int redHi = hiColor & 0x1f;
        int redLo = loColor & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int lerp = (int)(((unsigned int)(flags << 29)) >> 31);
        int num = age - stop1;
        int denom = stop2 - stop1;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (u16)(red | (green << 5) | (blue << 10));
        return;
    }
    if (age < stop3)
    {
        u16 loColor = header->color;
        u16 hiColor = ramp->endColor;
        u16 flags = ramp->flags;
        int redLo = loColor & 0x1f;
        int redHi = hiColor & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int lerp = (int)(((unsigned int)(flags << 29)) >> 31);
        int num = age - stop2;
        int denom = stop3 - stop2;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (u16)(red | (green << 5) | (blue << 10));
        return;
    }
    ptcl->color = ramp->endColor;
}
