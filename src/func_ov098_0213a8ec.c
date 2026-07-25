// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two;
// #pragma long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call.
#pragma long_calls on
extern int func_ov098_0213a8e0(void *b, void *c);

int func_ov098_0213a8ec(void *a, void *b, void *c)
{
    return func_ov098_0213a8e0(b, c);
}
