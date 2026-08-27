extern signed char data_0208e42c;
extern int data_0209b490;
extern int data_0209b494;
int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b){
  data_0208e42c = a;
  data_0209b494 = b;
  return data_0209b490 == (data_0208e42c << 12);
}
