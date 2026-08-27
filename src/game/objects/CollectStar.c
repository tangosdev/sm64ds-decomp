extern unsigned char data_0209cab4[];
extern unsigned char data_0209f224;
void CollectStar(int a, int b){
  data_0209cab4[a] |= 1 << b;
  data_0209f224 = b;
}
