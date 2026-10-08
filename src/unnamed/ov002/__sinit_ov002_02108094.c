struct S2 { int w[2]; };
extern struct S2 data_ov002_0210d600;
extern struct S2 data_ov002_0210d5f8;
struct S4 { struct S2 a; struct S2 b; };
extern struct S4 data_ov002_02111190;
void __sinit_ov002_02108094(void){
  data_ov002_02111190.a=data_ov002_0210d600;
  data_ov002_02111190.b=data_ov002_0210d5f8;
}
