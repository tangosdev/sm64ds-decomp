extern int func_0203d974(void);
extern void* func_02011e94(int* out);
extern void* _ZN4Heap14CreateRootHeapEPvj(void* start, unsigned int size);
extern void* data_0209d520;
void* func_0201a3e4(void){
  int local;
  if (func_0203d974()) {
    data_0209d520 = _ZN4Heap14CreateRootHeapEPvj(func_02011e94(&local), (unsigned int)local);
  }
  return data_0209d520;
}
