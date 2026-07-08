// NONMATCHING: register allocation (div=18). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned short u16;

extern void _ZN3IRQ11DisableIRQsEj(u32 mask);

typedef struct {
    void (*fn)(void *);   /* +0 */
    u32   flag;           /* +4 */
    void *arg;            /* +8 */
} DmaEnt;

extern DmaEnt _ZN3IRQ15DmaTimFunctionsE[];
extern u16 _ZN3IRQ13DmaTimIndicesE[];
extern volatile u32 reg[];

void _ZN3IRQ13DmaTimHandlerEv(u32 idx) {
    void (*fn)(void *) = _ZN3IRQ15DmaTimFunctionsE[idx].fn;
    u16 bit = _ZN3IRQ13DmaTimIndicesE[idx];
    u32 mask = 1u << bit;
    _ZN3IRQ15DmaTimFunctionsE[idx].fn = 0;
    if (fn != 0)
        fn(_ZN3IRQ15DmaTimFunctionsE[idx].arg);
    reg[0x3ff8 / 4] |= mask;
    if (_ZN3IRQ15DmaTimFunctionsE[idx].flag != 0)
        return;
    _ZN3IRQ11DisableIRQsEj(mask);
}
