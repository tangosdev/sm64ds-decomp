//cpp

extern "C" {

extern int data_ov007_02104bec[];

int func_ov007_020c1da0(int i) {
    return data_ov007_02104bec[i];
}

void func_ov007_020c1d8c(int i) {
    data_ov007_02104bec[i] = 1;
}

void func_ov007_020c1d78(int i) {
    data_ov007_02104bec[i] = 0;
}

} // extern "C"
