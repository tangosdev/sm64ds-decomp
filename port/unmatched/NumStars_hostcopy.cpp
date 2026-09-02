/* HOST COPY of src/NumStars.c -- the same loop, with the course id widened
 * where the two matched TUs disagree about its width.
 *
 * AN ABI RIDE-THROUGH, and the first one the port has hit between two SRC
 * files rather than between src and the host.
 *
 *   src/NumStars.c declares     extern int IsStarCollected(s8, s32);
 *   src/IsStarCollected.c defines    int IsStarCollected(int r0, int r1)
 *
 * On ARM both agree in practice: the argument rides in r0 and the compiler
 * that produced the caller had already widened the byte, so the callee's
 * `ldrb [r0 + table]` reads what the caller meant. On x86 cdecl a `char`
 * parameter is pushed as four bytes of which only the low one is defined, and
 * MSVC does exactly that -- the emitted caller stores the loop counter as a
 * BYTE into its stack slot and pushes the whole DWORD:
 *
 *     88 7d fc    mov  byte ptr [ebp-4], bh      ; level = 0
 *     8b 7d fc    mov  edi, dword ptr [ebp-4]    ; ...pushed as an int
 *
 * The callee then indexes data_0209cab4 with the three stale bytes above it.
 * Measured: METAL_NET's InitResources is the first caller NumStars has ever
 * had on the port, and its first star lookup faulted at data_0209cab4 +
 * 0x75760a00.
 *
 * The fix is the widening the ROM's own code effectively performs, spelled
 * once here. Nothing else changes: same bounds, same bit test, same count.
 * It retires when the two TUs agree on the parameter type.
 */
extern "C" {

int IsStarCollected(int courseID, int starID);

// PORT_HOST_ABI: matched src passes the course id relying on the ARM r0 ride-through; MSVC cdecl pushes a byte with three stale bytes above it and faults the star lookup, so it is widened here
unsigned char NumStars(void)
{
    unsigned char count = 0;
    for (int level = 0; level < 0x1e; ++level)
        for (int star = 0; star < 8; ++star)
            if (IsStarCollected(level, star))
                ++count;
    return count;
}

}  /* extern "C" */
