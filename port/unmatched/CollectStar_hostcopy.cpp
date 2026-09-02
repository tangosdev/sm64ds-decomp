/* HOST COPY of src/CollectStar.c -- the same two writes, with the course id
 * taken at the width its only caller declares.
 *
 * THE SAME ABI RIDE-THROUGH port/unmatched/NumStars_hostcopy.cpp documents, and the
 * WRITE half of it. Two TUs, two opinions:
 *
 *   src/CollectStarInLevel.c   extern void CollectStar(s8 courseID, s32);
 *   src/CollectStar.c          void CollectStar(int a, int b);
 *
 * On ARM the byte rides in r0 and the caller had already extended it, so the
 * callee's `ldrb` reads what the caller meant and mwccarm reproduces the ROM
 * bytes with either declaration. On x86 cdecl MSVC stores an s8 argument as a
 * BYTE into the outgoing stack slot and pushes the whole dword, so `int a` is
 * one meaningful byte and three stale ones -- and CollectStar's body is
 *
 *     data_0209cab4[a] |= 1 << b;
 *
 * an unbounded indexed READ-MODIFY-WRITE. Measured: the first star the port
 * ever collected faulted at data_0209cab4 + 0x60e95014. NumStars' identical
 * bug was a read; this one writes into whatever it lands on.
 *
 * The fix is the one src/IsStarCollected.c already argues for in its own
 * header -- take the parameter as s8 so the callee extends the byte itself --
 * applied to the one function in the pair that did not get it. Nothing else
 * changes: same table, same bit, same second write. It retires when the two
 * TUs agree on the parameter type.
 */
extern "C" {

extern unsigned char data_0209cab4[];
extern unsigned char data_0209f224;

// PORT_HOST_ABI: matched src passes the course id via the ARM r0 ride-through; MSVC cdecl pushes a byte with three stale bytes above it and corrupts the indexed write, so it is taken as s8 here
void CollectStar(signed char courseID, int starID)
{
    data_0209cab4[courseID] |= 1 << starID;
    data_0209f224 = (unsigned char)starID;
}

}  /* extern "C" */
