extern int data_ov007_02103440;
extern int data_ov007_0210343c;
int func_ov007_020b7948(int x,int cond){
  int r;
  if(cond){ r = data_ov007_02103440; data_ov007_02103440 = r + x; }
  else { r = data_ov007_0210343c - x; data_ov007_0210343c = r; }
  return r;
}
