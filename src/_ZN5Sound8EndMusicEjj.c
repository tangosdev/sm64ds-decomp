extern int data_0209b4ac;
extern unsigned char CURR_PLAYER_ID;
extern int data_0208e43c;
extern int data_0208e444;
extern int data_0208e440;
extern int _ZN5Sound7FileRef5PTR_0E[];
extern void func_02011dcc(int* g, int x);
extern void func_0204fa2c(int* g, int z);
void _ZN5Sound8EndMusicEjj(unsigned int j1, unsigned int j2){
  if(data_0209b4ac < 0) return;
  if(j1 != CURR_PLAYER_ID) return;
  int e43c = data_0208e43c;
  int e444 = data_0208e444;
  data_0209b4ac = e43c;
  if(e444 >= 0) return;
  int e440 = data_0208e440;
  if(e440 >= 0){ func_02011dcc(_ZN5Sound7FileRef5PTR_0E, e440); return; }
  if(e43c >= 0){ func_02011dcc(_ZN5Sound7FileRef5PTR_0E, e43c); return; }
  func_0204fa2c(_ZN5Sound7FileRef5PTR_0E, 0);
}
