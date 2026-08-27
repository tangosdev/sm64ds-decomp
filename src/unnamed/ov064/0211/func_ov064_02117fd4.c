// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two;
// #pragma long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call.
#pragma long_calls on
extern int func_ov064_02117fb4(void *b, void *c);

int func_ov064_02117fd4(void *a, void *b, void *c)
{
    return func_ov064_02117fb4(b, c);
}
