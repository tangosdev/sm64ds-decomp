
void func_ov006_02105c88(char *c)
{
  int cnt;
  int n;
  int i;
  cnt = 0;
  if ((*((unsigned char *) (c + 0x4fe9))) != 0)
  {
    return;
  }
  ;
  for (i = 0; i < (*((int *) (c + 0x4cb8))); i++)
  {
    if ((*((unsigned char *) ((c + i) + 0x4f1e))) != (*((unsigned char *) ((c + i) + 0x4f42))))
    {
      cnt++;
      break;
    }
  }

  if (cnt != 0)
  {
    return;
  }
  *((int *) (c + 0x4ca8)) = 6;
  *((unsigned char *) (c + 0x4fe6)) = 1;
  *((short *) (c + 0x4ec0)) = 0x40;
}
