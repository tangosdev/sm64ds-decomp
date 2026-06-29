
extern int data_ov006_0212eed0[];
extern int data_ov006_0212ee88[];
void func_ov006_0211a048(char *c, int idx)
{
  int off = idx * 0x24;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1cd)) = 1;
  *((int *) ((c + 0x51b0) + off)) = (*((int *) ((c + 0x51b0) + off))) + data_ov006_0212eed0[*((unsigned char *) ((c + 0x51d2) + off))];
  if (off && off)
  {
  }
  *((int *) (((c + off) + 0x5000) + 0x1bc)) = -0x2800;
  *((int *) (((c + off) + 0x5000) + 0x1b8)) = data_ov006_0212ee88[*((unsigned char *) ((c + 0x51d2) + off))];
  *((short *) (((c + off) + 0x5100) + 0xc8)) = 0;
  *((unsigned char *) (((c + off) + 0x5000) + 0x1d1)) = 1;
}
