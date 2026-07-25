// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on
extern int func_0213a2cc(void *thisp, void *data);
extern char data_ov015_02114880[];

int _ZN11FallBlockWf16CleanupResourcesEv(void *thisp)
{
    return func_0213a2cc(thisp, data_ov015_02114880);
}
