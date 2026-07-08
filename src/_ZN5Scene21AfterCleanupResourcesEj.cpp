//cpp
typedef unsigned int u32;

extern "C" {
  unsigned char data_02092660;
  void _ZN9ActorBase21AfterCleanupResourcesEj(void* thiz, u32 x);
}

extern "C" void _ZN5Scene21AfterCleanupResourcesEj(void* thiz, u32 x)
{
  if (x == 2)
    data_02092660 = 0;
  _ZN9ActorBase21AfterCleanupResourcesEj(thiz, x);
}
