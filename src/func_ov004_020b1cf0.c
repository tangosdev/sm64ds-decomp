void func_ov004_020afdd0(int a, int b, int c, int d, int e);
void func_ov004_020af68c(int a, int b, int c, int d, int e);
void RenderOamMainScreen(int a, int b, int c, int d, int e);
void func_ov004_020b1cf0(int a, int b, int c, int sel, int e){
  if(sel == -1){ func_ov004_020afdd0(a,b,c,-1,e); return; }
  if(sel == 0){ func_ov004_020af68c(a,b,c,-1,e); return; }
  RenderOamMainScreen(a,b,c,-1,e);
}
