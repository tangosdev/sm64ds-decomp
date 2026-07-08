extern int data_0209b4ac;
extern int data_0208e444;
extern int data_0208e43c;
extern int data_0208e440;
extern int _ZN5Sound7FileRef5PTR_0E[];
extern void func_02011dcc(int* g, int x);
extern void func_0204fa2c(int* g, int z);
void _ZN5Sound22StopLoadedMusic_Layer3Ev(void){
  int a = data_0209b4ac;
  if(a < 0){
    if(data_0208e444 < 0) return;
  }
  data_0208e444 = -1;
  int b = data_0208e43c;
  if(a == b){
    int e440 = data_0208e440;
    if(e440 >= 0){ func_02011dcc(_ZN5Sound7FileRef5PTR_0E, e440); return; }
  }
  if(a < 0 || b < 0)
    func_0204fa2c(_ZN5Sound7FileRef5PTR_0E, 0);
  else
    func_02011dcc(_ZN5Sound7FileRef5PTR_0E, a);
}
