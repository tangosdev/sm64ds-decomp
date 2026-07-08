extern unsigned char NUM_PLAYERS;
extern short NUM_COINS[];
short NumCoins(void){
  int i = 1;
  int n = NUM_PLAYERS;
  short* p = NUM_COINS;
  short sum = p[0];
  if (n <= 1) return sum;
  do {
    sum = sum + p[i];
    i++;
  } while (i < n);
  return sum;
}
