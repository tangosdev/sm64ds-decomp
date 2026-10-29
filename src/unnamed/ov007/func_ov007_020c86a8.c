extern void func_ov007_020c86c4(void *c);

/* The 0x34-byte screen transition record func_ov007_020c8b10 allocates
 * (data_ov007_02104bd8 slot 0); names as in func_ov007_020c8498 and
 * func_ov007_020c8ac0, only the slots this file writes. */
typedef struct Ov007Transition {
    char pad_00[4];
    unsigned int screens; /* 0x04: bit 0 main engine, bit 1 sub */
    char pad_08[0xc];
    int t; /* 0x14: Q12 progress */
    char pad_18[4];
    int reversed; /* 0x1c: 1 runs the curve backwards (func_ov007_020c8688) */
} Ov007Transition;

typedef char Ov007Transition_reversed_must_be_at_0x1c[(int)&((Ov007Transition *)0)->reversed == 0x1c
                                                          ? 1
                                                          : -1];

/* Starts the transition forwards on screens v (func_ov007_020c8688 is the
 * backwards twin). c keeps the callers' void * spelling; `tr` is the typed
 * view. */
void func_ov007_020c86a8(void *c, int v) {
    Ov007Transition *tr = (Ov007Transition *)c;
    tr->reversed = 0;
    tr->screens = v;
    tr->t = 0;
    func_ov007_020c86c4(c);
}
