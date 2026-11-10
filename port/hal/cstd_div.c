/* Host implementation of NitroSDK's FX_Div fixed-point divide.
 *
 * On the DS, FX_Div feeds the hardware divider (FX_DivAsync writes the
 * DIV registers, FX_GetDivResult spins on DIV_BUSY) -- src/cstd.cpp's FX_Div
 * is a thin wrapper over MMIO and cannot run on a host. Targets that hostgen
 * src/cstd.cpp (gate 8 and up) get FX_Div from it and do not link this file. The operation itself is
 * just a 20.12 divide: (a / b) in Fix12 is (a << 12) / b.
 *
 * DS divider edge cases, preserved deliberately:
 *   b == 0  -> the hardware yields +/-1 with the sign of the numerator (and
 *              sets DIV_DIV0); callers in this codebase never rely on it, but
 *              matching the hardware costs one branch and removes a class of
 *              "host differs on garbage input" surprises.
 */
typedef int s32;

s32 FX_Div(s32 a, s32 b)
{
    if (b == 0)
        return a < 0 ? -1 : 1;
    return (s32)(((long long)a << 12) / b);
}
