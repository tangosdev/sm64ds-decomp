extern int func_ov004_020b2444(int,int,int,int,int,int,int);
typedef struct { char pad[0x2f9]; unsigned char on; } BrFlag672c;
typedef struct { char pad[0xee]; unsigned short v; } BrEnt672c;
void func_ov006_020d672c(int c){
  if(((BrFlag672c*)(c+0x6000))->on==0) return;
  func_ov004_020b2444(0x80,0xc,((BrEnt672c*)(c+0x6200))->v,1,-1,0,0);
}
