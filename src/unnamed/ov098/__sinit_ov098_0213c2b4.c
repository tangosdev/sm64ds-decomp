extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov098_0213c91c[];
extern int data_ov098_0213c924[];
struct P2 { int a, b; };
extern struct P2 data_ov098_0213c724;
extern struct P2 data_ov098_0213c72c;
extern struct P2 data_ov098_0213c71c;
extern struct P2 data_ov098_0213c930[3];
void __sinit_ov098_0213c2b4(void){
  func_02017acc(data_ov098_0213c91c, 0x42c);
  func_020731dc(data_ov098_0213c91c, &func_02017ab4, data_ov098_0213c924);
  data_ov098_0213c930[0] = data_ov098_0213c724;
  data_ov098_0213c930[1] = data_ov098_0213c72c;
  data_ov098_0213c930[2] = data_ov098_0213c71c;
}
