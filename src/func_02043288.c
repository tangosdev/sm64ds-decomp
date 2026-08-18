/* FLOOR under 1.2/sp2p3 — same PTMF below-sp staging wall as func_0204322c (size 0x68 vs 0x5c). */
typedef struct { int a; int b; } PMF;
extern int _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(void *self, PMF a, PMF b, PMF c);
extern PMF data_02099eb4;
extern PMF data_02099eac;
extern PMF data_02099ea4;

int func_02043288(void *self)
{
    return _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(
        self, data_02099ea4, data_02099eac, data_02099eb4);
}
