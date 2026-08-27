extern unsigned char data_0209f21c;
extern short data_0209f358[];
short NumCoins(void){
  int i = 1;
  int n = data_0209f21c;
  short* p = data_0209f358;
  short sum = p[0];
  if (n <= 1) return sum;
  do {
    sum = sum + p[i];
    i++;
  } while (i < n);
  return sum;
}
