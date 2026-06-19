typedef signed char s8;
extern s8 data_0209f2f8;
extern s8 data_0208e420;
extern s8 data_0208e42c;
extern s8 data_0209b470;
extern s8 data_0208e424;
extern int data_0209b494;
extern int func_0204f958(int,int);
extern void func_02011c54(void);
extern int func_0204f9c4(int,int);
void _ZN5Sound10PauseMusicEv(void){
 int state=data_0209f2f8;
 if(state==2||state==5||state==4||state==0x32){
   if(data_0208e420<0){
     data_0208e420=data_0208e42c;
     data_0208e424=data_0209b470;
     data_0208e42c=0x40;
     data_0209b470=0x40;
     data_0209b494=0xc999;
   }
 } else {
   func_0204f958(0,1);
   func_0204f958(1,1);
 }
 func_02011c54();
 func_0204f9c4(0xd,1);
}
