typedef struct { void *a; void *b; } Pair;

struct Dst3 { int z0; void *s0; void *s1; };
struct Dst4 { int z0; int z4; void *s0; void *s1; };

extern Pair data_02099f60, data_02099f70, data_02099f48, data_02099f68, data_02099f50;

extern struct Dst3 data_020a4b6c;
extern struct Dst4 data_020a4b88, data_020a4b78, data_020a4b98, data_020a4ba8;

void __sinit_02075154(void) {
    data_020a4b6c.s0 = data_02099f60.a;
    data_020a4b6c.s1 = data_02099f60.b;
    data_020a4b6c.z0 = 0;

    data_020a4b88.z0 = 0;
    data_020a4b88.z4 = 0;
    data_020a4b88.s0 = data_02099f70.a;
    data_020a4b88.s1 = data_02099f70.b;

    data_020a4b78.z0 = 0;
    data_020a4b78.z4 = 0;
    data_020a4b78.s0 = data_02099f48.a;
    data_020a4b78.s1 = data_02099f48.b;

    data_020a4b98.z0 = 0;
    data_020a4b98.z4 = 0;
    data_020a4b98.s0 = data_02099f68.a;
    data_020a4b98.s1 = data_02099f68.b;

    data_020a4ba8.z0 = 0;
    data_020a4ba8.z4 = 0;
    data_020a4ba8.s0 = data_02099f50.a;
    data_020a4ba8.s1 = data_02099f50.b;
}
