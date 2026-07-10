/* func_020524e4 @ 0x20524e4 (arm9, Thumb) -- stores (b, a, -a, b) into p[0..3].
 * str r2,[r0]; str r1,[r0,#4]; negs r1,r1; str r1,[r0,#8]; str r2,[r0,#0xc]; bx lr
 */
#pragma thumb on
typedef int s32;

void func_020524e4(s32* p, s32 a, s32 b) {
    p[0] = b;
    p[1] = a;
    p[2] = -a;
    p[3] = b;
}
