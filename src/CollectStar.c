extern unsigned char data_0209cab4[];
extern unsigned char STAR_OBTAINED_COPY;
void CollectStar(int a, int b){
  data_0209cab4[a] |= 1 << b;
  STAR_OBTAINED_COPY = b;
}
