extern unsigned char CONNECTION_ERROR;
extern int func_020321fc(void);
extern void func_020326ac(void);
void func_02032f54(void){
  if (CONNECTION_ERROR == 0) return;
  if (func_020321fc() == 0) return;
  func_020326ac();
}
