extern int SOUND_SPECIFICS;
extern int JRB_SOUND_SPECIFICS;
extern int data_0209b48c;
extern int data_0208e430;
extern int data_0209b4ac;
extern int data_0208e43c;
extern int _ZN5Sound7FileRef5PTR_0E[];
extern int data_0209b4b0[];
extern void func_0204fa2c(int* g, int z);
extern void func_02011dcc(int* g, int x);
extern void func_02013524(int* g, int a, int b);
extern void func_02011b7c(void);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int j){
  SOUND_SPECIFICS=-1;
  JRB_SOUND_SPECIFICS=-1;
  data_0209b48c=0;
  if(j<0){
    func_0204fa2c(_ZN5Sound7FileRef5PTR_0E, 0);
  } else {
    if(!(data_0208e430>=0 && data_0209b4ac>=0 && data_0208e43c==j))
      func_02011dcc(_ZN5Sound7FileRef5PTR_0E, j);
    func_02013524(_ZN5Sound7FileRef5PTR_0E, 0x7f, 0);
    func_0204fa2c(data_0209b4b0, 0);
  }
  data_0209b4ac=j;
  data_0208e43c=j;
  func_02011b7c();
}
