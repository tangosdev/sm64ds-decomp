//cpp
struct Animation {
    int Finished();
};

int Animation::Finished()
{
    void* c = (void*)this;
unsigned int f=*(unsigned int*)((char*)c+4);
  int cur=*(int*)((char*)c+8);
  return cur>=(int)((f&0x3fffffff)-1);

}
