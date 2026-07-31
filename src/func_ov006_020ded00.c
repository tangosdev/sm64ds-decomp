
extern unsigned char DecIfAbove0_Byte(unsigned char *counter);
void func_ov006_020ded00(int this)
{
  unsigned long long new_var;
  *((int *) (this + 0)) += *((int *) (this + 8));
  *((int *) ((unsigned long long) (this + 4))) += *((int *) (this + 0xc));
  if (*((unsigned char *) (this + 0x13)))
  {
    *((int *) (this + 4)) -= 6;
  }
  if (DecIfAbove0_Byte((unsigned char *) ((this + 0x14))))
  {
    return;
  }
  *((unsigned char *) (this + 0x14)) = 3;
  *((signed char *) (new_var = this + 0x12)) = (*((signed char *) new_var)) - 1;
  if ((*((signed char *) (this + 0x12))) < 0)
  {
    *((signed char *) (this + 0x15)) = 0;
  }
}
