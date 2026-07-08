extern unsigned char CURR_PLAYER_ID;
extern int data_0209b4ac;
extern int data_0208e43c;
extern int data_0208e444;
extern int _ZN5Sound7FileRef5PTR_0E[];
extern void func_02011dcc(int* g);
void _ZN5Sound8SetMusicEjj(unsigned int j1, unsigned int j2){
  if(j1 != CURR_PLAYER_ID) return;
  if(data_0209b4ac < 0){
    if(data_0208e43c >= 0) return;
  }
  int e = data_0208e444;
  data_0209b4ac = j2;
  if(e >= 0) return;
  func_02011dcc(_ZN5Sound7FileRef5PTR_0E);
}
