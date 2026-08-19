extern unsigned char DecIfAbove0_Byte(unsigned char *counter);
void func_ov006_020ded00(int this)
{
  *((int *) (this + 0)) += *((int *) (this + 8));
  *((int *) (this + 4)) += *((int *) (this + 0xc));
  if (*((unsigned char *) (this + 0x13)))
  {
    *((int *) (this + 4)) -= 6;
  }
  if (DecIfAbove0_Byte((unsigned char *) (this + 0x14)))
  {
    return;
  }
  *((unsigned char *) (this + 0x14)) = 3;
  /* The two spellings of +0x12 are load-bearing under 2004/b56 and must not be
     unified: the ROM keeps the decrement's address in a register (`add r1,r4,#0x12`)
     and then RE-READS the byte as `ldrsb r0,[r4,#0x12]`. Written with one expression
     shape, b56 recognises the second read as the first lvalue and reuses r1. Every
     way of writing the decrement produces the ROM's form; only the re-read decides. */
  *((signed char *) (this + 0x12)) -= 1;
  if (*((signed char *) this + 0x12) < 0)
  {
    *((signed char *) (this + 0x15)) = 0;
  }
}
