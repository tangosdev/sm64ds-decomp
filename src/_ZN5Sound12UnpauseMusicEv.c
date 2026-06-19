extern signed char data_0209f2f8;
extern signed char data_0208e420;
extern signed char data_0208e424;
extern signed char data_0208e42c;
extern signed char data_0209b470;
extern void func_0204f958(int a, int b);
void _ZN5Sound12UnpauseMusicEv(void){
  signed char s = data_0209f2f8;
  if(s==2 || s==5 || s==4 || s==0x32){
    signed char lr = data_0208e420;
    if(lr < 0) return;
    data_0208e42c = lr;
    signed char ip = data_0208e424;
    data_0209b470 = ip;
    data_0208e420 = -1;
    return;
  }
  func_0204f958(0,0);
  func_0204f958(1,0);
}
