#ifdef _MSC_VER
/* THE INDEX IS WIDENED AT THE CALL, AS THE ROM DOES IT. func_ov002_020f5dd8
   (src/func_ov002_020f5dd8.cpp) takes the state index as a full int and
   indexes its pointer-to-member table with it. On ARM the byte below arrives
   zero-extended, because the ROM loads it with ldrb. MSVC passes an unsigned
   char argument by pushing the whole register after a byte load
   (mov al,[eax] / push eax), so the callee read the upper 24 bits of the
   script pointer as part of the index and jumped through a cell far past the
   table: the ending cutscene's first object command (script data_020876fc,
   command 0x2e, sub-command 21) froze the Stage. Declared int, the call
   zero-extends the byte here the way ldrb does. */
extern void func_ov002_020f5dd8(void *p, int v);
#else
extern void func_ov002_020f5dd8(void *p, unsigned char v);
#endif

int func_ov002_020f63f8(char *c, unsigned char *src)
{
    *(unsigned char*)(c + 0x100) = *src;
    func_ov002_020f5dd8(*(void**)(c + 0xd8), *(unsigned char*)(c + 0x100));
    return 1;
}
