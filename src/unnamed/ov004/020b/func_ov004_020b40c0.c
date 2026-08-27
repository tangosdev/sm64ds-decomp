extern int data_ov004_020bc254[];
int func_ov004_020b40c0(int* o){
  int* d = data_ov004_020bc254;
  if(o[0] == d[0] && (o[1] == d[1] || o[0] == 0) && o[9] == o[10])
    return 1;
  return 0;
}
