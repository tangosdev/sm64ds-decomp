// Gate-1 smoke: ABI assertions + known-value tests over the real src/ code.
// Every check here runs actual decompiled game code on the host -- no
// emulator, no ROM. A failure means either the host ABI diverges from the
// recovered layout or a seam implementation is wrong.
#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "Timer.h"
#include "Fader.h"
#include "FaderBrightness.h"

// ---- ABI: the recovered layouts must survive the host compiler ----------
static_assert(sizeof(u8) == 1 && sizeof(u16) == 2 && sizeof(u32) == 4, "int widths");
static_assert(sizeof(s64) == 8, "s64");
static_assert(sizeof(void *) == 4, "32-bit host (see port/platform.h)");
static_assert(sizeof(Fix12i) == 4, "Fix12i is a 32-bit 20.12 scalar");
static_assert(offsetof(Timer, mTime) == 0, "Timer +0x0");
static_assert(sizeof(((Timer *)0)->mTime) == 8, "Timer tick count is s64");
static_assert(offsetof(Timer, mIsRunning) == 8, "Timer +0x8");
// Fader's evidence header pins currInterp at +0x4 with the vptr at +0x0.
// This only holds if the host vptr is exactly 4 bytes -- x86-32 MSVC -- which
// is the whole 32-bit-first argument in one line:
static_assert(offsetof(Fader, currInterp) == 4, "vptr must be 4 bytes");
static_assert(offsetof(Fader, speed) == 8, "Fader +0x8");

// ---- the C math slice ---------------------------------------------------
// Vector3 comes from the shared headers (common.h via the Fader includes);
// redefining it here was a redefinition error, which is itself a small ABI
// win: the test uses the same type the game does.
extern "C" Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
extern "C" void CrossVec3(const Vector3 *a, const Vector3 *b, Vector3 *out);
extern "C" void sm64ds_hal_advance_ticks(s64 n);
int ApproachLinear(int &ref, int target, int step);

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

#define ONE 0x1000  /* 1.0 in 20.12 */

static void test_math(void)
{
    Vector3 x = {ONE, 0, 0}, y = {0, ONE, 0}, out;

    CHECK(DotVec3(&x, &x) == ONE);            /* 1.0 . 1.0 */
    CHECK(DotVec3(&x, &y) == 0);              /* orthogonal */
    Vector3 half = {ONE / 2, 0, 0};
    CHECK(DotVec3(&x, &half) == ONE / 2);     /* 1.0 * 0.5 */

    CrossVec3(&x, &y, &out);                  /* x cross y = z */
    CHECK(out.x == 0 && out.y == 0 && out.z == ONE);

    int v = 0;
    CHECK(ApproachLinear(v, 0x1000, 0x400) == 0 && v == 0x400);
    v = 0xF00;
    CHECK(ApproachLinear(v, 0x1000, 0x400) == 1 && v == 0x1000);  /* clamps */
    v = 7;
    CHECK(ApproachLinear(v, 7, 0) == 1);      /* zero step, at target */
}

static void test_timer(void)
{
    Timer t;
    t.ResetTimer();
    CHECK(t.mIsRunning == 0 && t.mTime == 0);
    CHECK(t.GetTime() == 0);

    t.StartTimer();
    CHECK(t.mIsRunning == 1);
    sm64ds_hal_advance_ticks(1000);
    CHECK(t.GetTime() == 1000);               /* running: now - start */
    t.StopTimer();
    CHECK(t.mIsRunning == 0 && t.GetTime() == 1000);   /* frozen */
    sm64ds_hal_advance_ticks(500);
    CHECK(t.GetTime() == 1000);               /* still frozen */
}

static void test_fader(void)
{
    // Host-constructible subclass; the base has no ctor of its own in the
    // slice. The vtable this creates is the HOST's -- which is the point:
    // the port uses real C++ dispatch where the hybrid must trampoline.
    struct TestFader : FaderBrightness {} f;

    f.SetToStart();
    CHECK(f.IsAtStart() == 1 && f.IsAtEnd() == 0);
    CHECK(f.IsBetweenStartAndEnd() == 0);

    // 16-frame forward fade: speed = fdiv(1.0, 16.0) through the HAL divider
    f.SetForwardTime(16);
    CHECK(f.speed == ONE / 16);
    for (int i = 0; i < 15; ++i) {
        f.AdvanceInterp();
        CHECK(f.IsBetweenStartAndEnd() == 1);
    }
    f.AdvanceInterp();                        /* 16th step lands exactly */
    CHECK(f.IsAtEnd() == 1 && f.currInterp == ONE);

    f.SetBackwardTime(8);                     /* and back down */
    for (int i = 0; i < 8; ++i)
        f.AdvanceInterp();
    CHECK(f.IsAtStart() == 1 && f.currInterp == 0);
}

int main(void)
{
    test_math();
    test_timer();
    test_fader();
    if (g_failures) {
        fprintf(stderr, "smoke: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke: all checks passed (math, Timer, Fader on host)\n");
    return 0;
}
