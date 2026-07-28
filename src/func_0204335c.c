/* FLOOR under 1.2/sp2p3 — same PTMF below-sp staging wall as func_0204322c (size 0x68 vs 0x5c). */
typedef struct { int a; int b; } PMF;
extern int _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(void *self, PMF a, PMF b, PMF c);
extern PMF data_02099e94;
extern PMF data_02099ec4;
extern PMF data_02099ebc;

int func_0204335c(void *self)
{
    return _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(
        self, data_02099ebc, data_02099ec4, data_02099e94);
}
