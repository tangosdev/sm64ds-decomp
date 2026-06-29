struct S2 { int w[2]; };
extern struct S2 data_ov081_02128a6c;
extern struct S2 data_ov081_02128a74;
struct S4 { struct S2 a; struct S2 b; };
extern struct S4 data_ov081_02128eb4;
void __sinit_ov081_021284b4(void){
  data_ov081_02128eb4.a=data_ov081_02128a6c;
  data_ov081_02128eb4.b=data_ov081_02128a74;
}
