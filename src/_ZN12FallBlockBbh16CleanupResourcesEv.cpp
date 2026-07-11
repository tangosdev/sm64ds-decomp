//cpp
// Tail-call veneer: load r1 with the class resource data pointer and jump to the
// shared cleanup helper through ip (this stays in r0). Hand-asm because the
// trailing .words are relocations (wildcards) and mwccarm would otherwise emit a
// near `b` instead of the ROM's pooled `ldr ip,[pc]; bx ip` tail-call.
extern "C" {
extern void func_ov098_0213a2cc(void);
extern char data_ov063_0211eb10[];

asm int _ZN12FallBlockBbh16CleanupResourcesEv(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov098_0213a2cc
    dcd data_ov063_0211eb10
}
}
