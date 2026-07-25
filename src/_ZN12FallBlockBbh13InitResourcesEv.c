// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on
extern int func_ov098_0213a794(void *thisp, void *data);
extern char data_ov063_0211eb10[];

int _ZN12FallBlockBbh13InitResourcesEv(void *thisp)
{
    return func_ov098_0213a794(thisp, data_ov063_0211eb10);
}
