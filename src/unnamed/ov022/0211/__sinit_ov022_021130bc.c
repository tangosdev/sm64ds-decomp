struct S2 { int w[2]; };
extern struct S2 data_ov022_0211442c;
extern struct S2 data_ov022_02114424;
struct S4 { struct S2 a; struct S2 b; };
extern struct S4 data_ov022_02114690;
void __sinit_ov022_021130bc(void){
  data_ov022_02114690.a=data_ov022_0211442c;
  data_ov022_02114690.b=data_ov022_02114424;
}
