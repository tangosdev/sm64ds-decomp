//cpp
struct Animation {
    void SetFlags(int flags);
};

void Animation::SetFlags(int flags)
{
    void* c = (void*)this;
*(unsigned int*)((char*)c+4)=(*(unsigned int*)((char*)c+4)&0x3fffffff)|flags;

}
