/* FLOOR under 1.2/sp2p3: mwccarm always emits push{fp,lr}+mov sp,ip dynamic frame
 * (cand size 0x68 vs ROM 0x5c). Documented wall: notes/mwccarm-codegen.md PTMF wrappers.
 * 2.0/sp2p3 -proc arm7tdmi: 1-word near-miss (ldr r3,[ip] vs ROM ldm ip,{r3}).
 * Tried: C struct-by-value, six-int, local copies, stack arrays, u64, union, nested,
 * //cpp real PTMF, pragma size — all fp-frame under pin 1.2/sp2p3.
 */
typedef struct { int a; int b; } PMF;
extern int _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(void *self, PMF a, PMF b, PMF c);
extern PMF data_02099e9c;
extern PMF data_02099e74;
extern PMF data_02099ecc;

int func_0204322c(void *self)
{
    return _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(
        self, data_02099ecc, data_02099e74, data_02099e9c);
}
