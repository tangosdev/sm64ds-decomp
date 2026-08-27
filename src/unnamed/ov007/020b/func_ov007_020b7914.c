extern int data_ov007_02103438;
extern int data_ov007_02103434;
int func_ov007_020b7914(int x,int cond){
  int r;
  if(cond){ r = data_ov007_02103438; data_ov007_02103438 = r + x; }
  else { r = data_ov007_02103434 - x; data_ov007_02103434 = r; }
  return r;
}
