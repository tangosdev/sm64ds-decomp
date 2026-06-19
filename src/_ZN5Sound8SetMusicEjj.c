extern unsigned char data_0209f250;
extern int data_0209b4ac;
extern int data_0208e43c;
extern int data_0208e444;
extern int data_0209b4a0[];
extern void func_02011dcc(int* g);
void _ZN5Sound8SetMusicEjj(unsigned int j1, unsigned int j2){
  if(j1 != data_0209f250) return;
  if(data_0209b4ac < 0){
    if(data_0208e43c >= 0) return;
  }
  int e = data_0208e444;
  data_0209b4ac = j2;
  if(e >= 0) return;
  func_02011dcc(data_0209b4a0);
}
