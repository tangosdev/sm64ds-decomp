// Argument-loading long-branch veneer: load the target address into ip and a data
// pointer argument into r1 from the two inline literals, then tail-jump via `bx ip`.
// Hand-asm because both trailing .words are relocations (wildcards) and the placeholder
// symbol must stay unmangled.
extern void func_ov002_020b6244(void);
extern char data_ov064_0211adb0[];
asm void func_ov064_021180e8(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov002_020b6244
    dcd data_ov064_0211adb0
}
