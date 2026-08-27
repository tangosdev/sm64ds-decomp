// func_02052550 @ 0x02052550 (arm9, Thumb): build 3x3 fx32 X-rotation matrix
// from (sin, cos): {1, 0, 0, 0, c, s, 0, -s, c} with 4096 = 1.0.
// The named `one` local + opt_propagation off keeps 1<<12 materialized as
// movs #1; lsls #12 instead of a literal-pool load.
#pragma thumb on
#pragma opt_propagation off
typedef int s32;

void func_02052550(s32 *m, s32 s, s32 c) {
    s32 one = 1;
    m[0] = one << 12;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = c;
    m[5] = s;
    m[6] = 0;
    m[7] = -s;
    m[8] = c;
}
