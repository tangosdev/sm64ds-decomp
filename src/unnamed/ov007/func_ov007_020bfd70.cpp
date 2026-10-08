//cpp
// @symbol func_ov007_020bfd70
/* recovered: project a world point into DS screen pixels for an ov007 actor.
 *
 * The point at `pos` is transformed by the actor's 4x3 matrix at +0x44 into a local
 * camera-space triple. The actor's 16-bit angle at +0xd4, reduced by >> 4, indexes the
 * arm9 SINE_TABLE (data_02082214) in PAIRS: the even half multiplies the absolute depth
 * out[2] to give the denominator, and cstd::fdiv divides the odd half by it. Everything
 * is 1.12 fixed point, so each product is rounded with + 0x800 and shifted down by 12.
 * The resulting factor scales the x and y components, and the last two lines land them
 * on the screen: x times 0x60 about the 0x80 centre column, y negated about the 0x60
 * centre row.
 *
 * ONE SPELLING IS LOAD-BEARING and it is a declaration RANK, not a type (notes 6cc/6k):
 * `z` and its absolute-value fix-up are read FIRST, ahead of the angle, the table base
 * and the two table reads. With `z` read in its natural place next to the table reads
 * the function is still exactly 0xdc bytes with every instruction shape, immediate and
 * branch right, and seven words differ as a pure four-register rotation (the pool base
 * ip vs r0, `j` r2 vs ip, the second byte offset lr vs ip, the 0x800 constant r0 vs r1).
 * Hoisting the `z` pair to the top of the block is the whole match; widening any local
 * to `long` on top of it is byte-inert, and no launder, transposition or pragma reaches
 * it (492-cell pragma sweep and ~6,000 permuter iterations, both flat at 7).
 */
extern "C" {
void MulVec3Mat4x3(void* v, void* m, void* out);
int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];

void func_ov007_020bfd70(char* actor, void* pos, int* outX, int* outY) {
    int out[3];
    MulVec3Mat4x3(pos, actor + 0x44, out);
    int z = out[2];
    if (z < 0) z = -z;
    int ang = *(unsigned short*)(actor + 0xd4) >> 4;
    int i = ang << 1;
    short* base = &data_02082214[0];
    int j = i + 1;
    int cosv = base[i];
    int sinv = base[j];
    int r = (int)(((long long)cosv * (long long)z + 0x800) >> 0xc);
    int f = _ZN4cstd4fdivEii(sinv, r);
    int a = (int)(((long long)f * (long long)out[1] + 0x800) >> 0xc);
    int b = (int)(((long long)f * (long long)out[0] + 0x800) >> 0xc);
    outX[0] = ((b * 0x60) >> 0xc) + 0x80;
    outY[0] = ((-a * 0x60) >> 0xc) + 0x60;
}
}
