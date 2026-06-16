//cpp
namespace cstd { int fdiv(int,int); }
extern "C" int func_ov096_02135878(int unused, int x){
  if(x>0x190000) return 0;
  int q=cstd::fdiv(0x4000,0xc8000);
  long long m=(long long)q*x;
  m+=0x800;
  int r=(int)(m>>12);
  r=0x8000-r;
  return (short)r;
}
