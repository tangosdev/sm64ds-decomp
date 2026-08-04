extern int RandomIntInternal(void* seed);
extern int data_0209e650[];
int func_ov006_020ebe6c(void){
  int v = 3;
  int r = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
  if (r < 0x400) v = 0;
  else if (r < 0x800) v = 1;
  else if (r < 0xc00) v = 2;
  return v;
}
