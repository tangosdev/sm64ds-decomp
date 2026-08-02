/* Host implementation of the cstd fixed-point divide.
 *
 * On the DS, cstd::fdiv feeds the hardware divider (fdiv_async writes the
 * DIV registers, fdiv_result spins on DIV_BUSY) -- src/_ZN4cstd4fdivEii.c is
 * a thin wrapper over MMIO and cannot run on a host. The operation itself is
 * just a 20.12 divide: (a / b) in Fix12 is (a << 12) / b.
 *
 * DS divider edge cases, preserved deliberately:
 *   b == 0  -> the hardware yields +/-1 with the sign of the numerator (and
 *              sets DIV_DIV0); callers in this codebase never rely on it, but
 *              matching the hardware costs one branch and removes a class of
 *              "host differs on garbage input" surprises.
 */
typedef int s32;

s32 _ZN4cstd4fdivEii(s32 a, s32 b)
{
    if (b == 0)
        return a < 0 ? -1 : 1;
    return (s32)(((long long)a << 12) / b);
}
