/* hal/pmf_tc.h -- a thiscall entry over a flat cdecl(this) function.
 *
 * PORTSYNC3. Main's class TUs dispatch their state tables natively
 * (`(this->*cell)()`): under /vmg /vmm that is a call with `this` in ECX and nothing
 * on the stack. The port's seats write the cell's function word from the flat ROM
 * name, which the face ledger defines as a cdecl function reading `this` off the stack
 * (the ROM-call-site shape). Entered natively it read garbage for `this`.
 *
 * port_pmf_tc(f) returns the address of a small stub that does what the native call
 * site expects of a member: it takes the receiver in ECX, pushes it for the cdecl
 * callee, calls, drops it, and returns the callee's EAX. One stub per function, made
 * once. Only for functions of the shape int/void f(void *this) -- every state record
 * cell is -- and never for one that takes more arguments.
 */
#ifndef PORT_PMF_TC_H
#define PORT_PMF_TC_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *port_pmf_tc(const void *flat);
void port_pmf_tc_cells(void *base, const int *idx, int n);   /* listed {fn,delta} cells holding flat functions */
/* The stubs sit at an address computed from the function they enter, in a range
   reserved at a fixed base, so one build puts each stub at the same address in
   every process and a disk save state's copies of them stay valid (hal/pmf_tc.cpp).
   For hal/lk7_persist.cpp: */
int  port_pmf_tc_pinned(void);       /* 1: every stub so far is at its pinned address */
unsigned port_pmf_tc_base(void);     /* the range base, 0 when not reserved */
int  port_pmf_tc_count(void);
void port_pmf_tc_entry(int i, unsigned *flat, unsigned *stub);
int  port_pmf_tc_rebuild(const unsigned *pairs, int n);   /* -1 ok, else first bad index */
#ifdef __cplusplus
}
#endif
#define PORT_PMF_TC(f) ((unsigned)(size_t)port_pmf_tc((const void *)(size_t)(f)))
#endif
