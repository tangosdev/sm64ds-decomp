extern void func_02045e2c(int obj, int i, int a, int b);
void func_020162a4(int obj, int i) {
    func_02045e2c(obj + 8, i, *(int *)(obj + 0x64),
                  (*(unsigned int *)(obj + 0x70) << 4) >> 16);
}
