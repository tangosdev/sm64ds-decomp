struct S2 { int w[2]; };
extern struct S2 data_ov055_02111a94;
extern struct S2 data_ov055_02111a9c;
struct S4 { struct S2 a; struct S2 b; };
extern struct S4 data_ov055_02111b70;
void __sinit_ov055_021118d4(void){
  data_ov055_02111b70.a=data_ov055_02111a94;
  data_ov055_02111b70.b=data_ov055_02111a9c;
}
