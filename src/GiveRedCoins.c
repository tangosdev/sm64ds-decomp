extern signed char NUM_RED_COINS[];
int GiveRedCoins(int i, int amt){
  signed char* p = &NUM_RED_COINS[i];
  NUM_RED_COINS[i] = (signed char)(NUM_RED_COINS[i] + amt);
  int v = NUM_RED_COINS[i];
  if(v > 0x63){ v = 0x63; *p = (signed char)v; return v; }
  if(v < 0){ v = 0; *p = (signed char)v; return v; }
  return v;
}
