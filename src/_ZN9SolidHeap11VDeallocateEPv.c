extern void Crash();
void _ZN9SolidHeap11VDeallocateEPv(void *c, void *a){
  if (a == 0) return;
  Crash();
}
