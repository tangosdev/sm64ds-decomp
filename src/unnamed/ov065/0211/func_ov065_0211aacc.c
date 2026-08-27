// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two.
// The pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call is just what mwccarm emits for a
// cross-section tail call here; it is NOT produced by a pragma. This file used to carry
// `#pragma long_calls on` with a comment crediting it, but long_calls is not a pragma
// mwccarm recognises (verified with -w illpragmas, see notes 6as) -- it was silently
// ignored, and the function matches on all 12 builds without it.
extern int func_ov065_0211aa38(void *b, void *c);

int func_ov065_0211aacc(void *a, void *b, void *c)
{
    return func_ov065_0211aa38(b, c);
}
