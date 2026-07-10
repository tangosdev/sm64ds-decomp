extern int func_ov004_020b0930(void*);
int _ZN17MgBounceAndPounce19BeforeInitResourcesEv(void* c){
  if(func_ov004_020b0930(c)==0) return 0;
  *(int*)((char*)c+0x5000)=0;
  return 1;
}
