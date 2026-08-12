// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on
extern int func_ov002_020b66a8(void *thisp, void *data);
extern char data_ov029_02113fd4[];

int _ZN19RotatingPlatformWdw16CleanupResourcesEv(void *thisp)
{
    return func_ov002_020b66a8(thisp, data_ov029_02113fd4);
}
