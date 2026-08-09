struct S2 { int w[2]; };
extern struct S2 data_ov026_02113dd8;
extern struct S2 data_ov026_02113dd0;
struct D4 { int w[4]; };
extern struct D4 data_ov026_02113f58;
void __sinit_ov026_02112d68(void){
  *(struct S2*)((char*)&data_ov026_02113f58+0)=data_ov026_02113dd8;
  *(struct S2*)((char*)&data_ov026_02113f58+8)=data_ov026_02113dd0;
}
