extern int* data_ov007_02104bbc;
extern int* data_ov007_02104bb8;
extern void func_ov007_020be098(int idx);
void func_ov007_020bde2c(int idx){
  if (data_ov007_02104bbc[idx] == 0) {
    func_ov007_020be098(idx);
  }
  data_ov007_02104bb8[idx-0x35] = 1;
}
