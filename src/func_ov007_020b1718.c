/* Title screen element state 4: the start sequence.
 *
 * Dispatched from src/func_ov007_020b0548.c case 4 once a tap has moved the
 * title's element state machine off state 0. Mode 0 (the scene's +0x20 word)
 * drives the three-axis pose interpolation that plays the start animation and
 * arms the state advance at frame 0x28; mode 1 is the cursor/pointer path,
 * which drives the +0xf8 fade instead and advances when its own frame pair
 * meets.
 *
 * Two constructs here are load-bearing for the byte match and are NOT
 * cosmetic. Both were measured, each on its own:
 *
 *  - `ease2` is a case-local, deliberately NOT the function-scope `remain`.
 *    The two never overlap, so sharing one variable is semantically identical,
 *    but sharing it welds one live range across the whole case and the
 *    allocator then colours the whole interpolation block one register off.
 *    Splitting the web is what lands the ROM's colouring.
 *  - `pose` re-reads m+0x3c after the m+0x14 store, as its own statement. The
 *    store can alias the pointer, so the ROM reloads; hoisting the reload to a
 *    named statement at that exact point is what puts it in the scratch
 *    register the ROM uses.
 *
 * The `(int)(unsigned int)` round trip on the second m+0x14 term is value
 * preserving (both terms compile to signed smull/smlal); without it the
 * function is one instruction longer.
 */
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020b257c(void);
extern void func_ov007_020b2454(void* a, void* b);
extern void func_ov007_020bce70(int a, int b);
extern int func_ov007_020c3be0(int a, int b, int c);
extern void func_ov007_020b2764(int a);

extern char* data_ov007_0210342c;

struct S12 { int a, b, c; };

void func_ov007_020b1718(void)
{
    char* obj = *(char**)(data_ov007_0210342c + 0xf4);
    int remain;
    int v;
    int done = 0;
    int span;
    int mode;

    v = *(int*)(*(char**)(data_ov007_0210342c + 8) + 0xc);

    if (v == 0)
        *(s16*)(*(char**)(data_ov007_0210342c + 0xc) + 2) = 2;

    if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 0)
        func_ov007_020bdeb0(0x1c);
    else if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 1)
        func_ov007_020bdeb0(0x1d);

    mode = *(int*)(data_ov007_0210342c + 0x20);
    switch (mode) {
    case 1: {
        char* cur = *(char**)(obj + 0x18);
        int ratio, phase, fade;
        char* cur2;

        span = 0x1e;
        remain = (int)*(u16*)(cur + 0x10) - ((int)*(u16*)(cur + 0x12) - 0x12);

        if (v < 0xc) {
            if (v <= 0) ratio = 0;
            else if (v >= 0xc) ratio = 0x1000;
            else ratio = _ZN4cstd3divEii(v << 0xc, 0xc);
            phase = ratio << 2;

            if (phase <= 0x1000)
                fade = (int)(((s64)phase * 0x1000 + 0x800) >> 12);
            else if (phase <= 0x3000)
                fade = (int)(((s64)(phase - 0x1000) * (-0x400) + 0x800) >> 12) + 0x1000;
            else
                fade = (int)((((s64)1 - (phase - 0x3000)) * 0x800) >> 12) + 0x800;

            *(int*)(data_ov007_0210342c + 0xf8) = 0x1000 - (fade >> 2);
        } else {
            *(int*)(data_ov007_0210342c + 0xf8) = 0x1000;
        }

        cur2 = *(char**)(obj + 0x18);
        if (*(s16*)(cur2 + 0xc) != -1) {
            if ((int)*(u16*)(cur2 + 0x10) == (int)*(u16*)(cur2 + 0x12) - 2) {
                if (*(int*)cur2 == 0)
                    *(int*)cur2 = 1;
                done = 1;
            }
        }
        break;
    }
    case 0: {
        char* rig = *(char**)(data_ov007_0210342c + 0x30);
        int ratio, ease1, sq, wave;
        int ease2;
        char* pose;

        if (v == 0) {
            char* d;
            func_ov007_020b257c();
            d = *(char**)(rig + 0x3c);
            *(struct S12*)(d + 0x20) = *(struct S12*)(rig + 0x14);
            d = *(char**)(rig + 0x3c);
            *(struct S12*)(d + 0x14) = *(struct S12*)(d + 0x20);
            *(int*)(*(char**)(rig + 0x40) + 0x18) = *(int*)(rig + 0x2c);
            d = *(char**)(rig + 0x40);
            *(int*)(d + 0x14) = *(int*)(d + 0x18);
            func_ov007_020b2454(*(char**)(rig + 0x3c) + 0x20, *(char**)(rig + 0x40) + 0x18);
        }

        if (v <= 0) ratio = 0;
        else if (v >= 0x1e) ratio = 0x1000;
        else ratio = _ZN4cstd3divEii(v << 0xc, 0x1e);

        ease1 = (int)(((s64)ratio * (0x2000 - ratio)) >> 12);
        ease2 = (int)(((s64)ease1 * (0x2000 - ease1)) >> 12);
        sq = (int)(((s64)ease2 * ease2) >> 12);
        if (sq >= 0x800)
            wave = 0x1000 - ((sq - 0x800) << 1);
        else
            wave = sq << 1;

        *(int*)(rig + 0x14) = (int)(((s64)(0x1000 - ease2) * *(int*)((*(char**)(rig + 0x3c)) + 0x14) + (s64)(int)(unsigned int)ease2 * *(int*)((*(char**)(rig + 0x3c)) + 0x20)) >> 12);
        pose = *(char**)(rig + 0x3c);
        wave = ease2 - (int)(((s64)wave * (0x2000 - wave)) >> 12);
        *(int*)(rig + 0x18) = (int)(((s64)(0x1000 - wave) * *(int*)(pose + 0x18) + (s64)wave * *(int*)(pose + 0x24)) >> 12);
        ratio = (int)(((s64)ease2 * (0x2000 - ease2)) >> 12);
        *(int*)(rig + 0x2c) = (int)(((s64)(0x1000 - ratio) * *(int*)((*(char**)(rig + 0x40)) + 0x14) + (s64)ratio * *(int*)((*(char**)(rig + 0x40)) + 0x18)) >> 12);
        remain = v - 2;
        span = 0x28;
        if (v >= 0x28)
            done = 1;
        break;
    }
    }

    if (done != 0) {
        *(int*)(data_ov007_0210342c + 0x1c) = 2;
        *(s16*)(*(char**)(data_ov007_0210342c + 4) + 2) = 1;
    }

    if (v >= 2 && remain >= 0) {
        int level;
        int step;
        func_ov007_020bce70(2, 0xf);
        level = *(int*)(*(char**)(data_ov007_0210342c + 0x40) + 0xc);
        if (level <= 0) step = 0;
        else if (level >= 0xf) step = 0x1000;
        else step = _ZN4cstd3divEii(level << 0xc, 0xf);
        *(u16*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x22) =
            func_ov007_020c3be0(step, *(u16*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x22), 0);
    }

    {
        int out;
        if (v <= 0) out = 0;
        else if (v >= span) out = 0x1000;
        else out = _ZN4cstd3divEii(v << 0xc, span);
        func_ov007_020b2764(0x1000 - out);
    }
}
