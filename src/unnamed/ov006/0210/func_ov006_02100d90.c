
void func_ov006_02100d90(char *base, int idx)
{
  char *r3 = base + (idx * 0x40);
  if ((*((unsigned short *) (r3 + 0x5292))) != 0)
  {
    unsigned short *h = (unsigned short *) ((base + 0x5292) + (idx * 0x40));
    *h = (*h) - 1;
    return;
  }
  char *q;
  *((int *) ((base + 0x5264) + (idx * 0x40))) = (*((int *) ((base + 0x5264) + (idx * 0x40)))) + (*((int *) ((r3 + 0x5000) + 0x26c)));
  *((int *) ((base + 0x526c) + (idx * 0x40))) = (*((int *) ((base + 0x526c) + (idx * 0x40)))) - 0x100;
  int a = *((int *) ((r3 + 0x5000) + 0x280));
  int b = *((int *) ((r3 + 0x5000) + 0x264));
  if (a < b)
  {
    return;
  }
  *((int *) ((r3 + 0x5000) + 0x264)) = a;
  *((unsigned char *) ((r3 + 0x5000) + (b = 0x296))) = *((unsigned char *) ((r3 + 0x5000) + 0x297));
  *((int *) ((r3 + 0x5000) + 0x268)) = *((int *) ((r3 + 0x5000) + 0x284));
  *((int *) ((r3 + 0x5000) + 0x26c)) = *((int *) ((r3 + 0x5000) + 0x288));
}
