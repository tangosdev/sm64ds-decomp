extern signed char data_0209f30c[];
int GiveRedCoins(int i, int amt){
  signed char* p = &data_0209f30c[i];
  data_0209f30c[i] = (signed char)(data_0209f30c[i] + amt);
  int v = data_0209f30c[i];
  if(v > 0x63){ v = 0x63; *p = (signed char)v; return v; }
  if(v < 0){ v = 0; *p = (signed char)v; return v; }
  return v;
}
