
typedef long long s64;
struct S;
extern void func_020551f0(struct S *s, int v);
inline int inline_fn(int arg0)
{
  return arg0;
}

/* The 0x34-byte screen transition record func_ov007_020c8b10 allocates
 * (data_ov007_02104bd8 slot 0); names as in func_ov007_020c8ac0, only the
 * slots this file reads: the brightness effect's (4) levels. */
typedef struct Ov007Transition {
    char pad_00[4];
    unsigned int screens; /* 0x04: bit 0 main engine, bit 1 sub */
    char pad_08[0x24];
    int from; /* 0x2c: fx.brightness.from */
    int to;   /* 0x30: fx.brightness.to */
} Ov007Transition;

typedef char Ov007Transition_to_must_be_at_0x30[(int)&((Ov007Transition *)0)->to == 0x30 ? 1 : -1];

/* Eases the master brightness from `from` to `to` by t (Q12) on the
 * transition's screens. c keeps the caller's char * spelling; `tr` is the
 * typed view. */
void func_ov007_020c8b34(char *c, int t)
{
    Ov007Transition *tr = (Ov007Transition *)c;
    int new_var2;
    int v = tr->to;
    int ip = tr->from;
    int new_var;
    if (t < 0x1000) {
        if (t <= 0) {
            v = ip;
        } else {
            int mid =
                (int)(((((s64)t) * (inline_fn(v) << 0xc)) + (((s64)(0x1000 - t)) * (ip << 0xc))) >>
                      0xc);
            new_var = mid;
            v = new_var >> 0xc;
        }
    }
    if (tr->screens & 1) {
        new_var2 = 0x4000050;
        func_020551f0((struct S *)new_var2, v);
    }
    if (tr->screens & 2) {
        func_020551f0((struct S *)0x4001050, v);
    }
}
