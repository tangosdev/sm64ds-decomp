struct S2 { int w[2]; };
extern struct S2 data_ov064_0211c2e4;
extern struct S2 data_ov064_0211c2dc;
struct S4 { struct S2 a; struct S2 b; };
extern struct S4 data_ov064_0211c934;
void __sinit_ov064_0211b4dc(void){
  data_ov064_0211c934.a=data_ov064_0211c2e4;
  data_ov064_0211c934.b=data_ov064_0211c2dc;
}
