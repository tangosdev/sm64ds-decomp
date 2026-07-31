//cpp
// @symbol func_ov089_02131f04
// @emits Key_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daObjKey_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern int func_ov089_02131df4(void *c, int a);
extern int func_ov089_02131dcc(void *c, int a);
int Key_OnTurnIntoEgg(char *c, int a){
  int h=*(unsigned short*)(c+0xc); int eq; if(h==0x11a) eq=1; else eq=0;
  if(eq) return func_ov089_02131df4(c,a); return func_ov089_02131dcc(c,a);
}
}
