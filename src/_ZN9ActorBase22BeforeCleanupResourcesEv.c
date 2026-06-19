extern int func_0204424c(int);
int _ZN9ActorBase22BeforeCleanupResourcesEv(char* c){
  int v=*(int*)(c+0x48);
  if(v!=0){
    if(func_0204424c(v)==0) goto ret0;
  }
  if(*(int*)(c+0x18)==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
