// Long-branch veneer: load absolute target into ip and data arg into r1, then bx ip.
extern void func_ov080_021274ac(void);
extern void *data_ov030_02115a04;
asm void _ZN9UkikiCage13InitResourcesEv(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov080_021274ac
    dcd data_ov030_02115a04
}
