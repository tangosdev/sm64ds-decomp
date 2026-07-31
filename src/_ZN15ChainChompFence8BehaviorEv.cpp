//cpp
extern "C" {
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
}

struct ChainChompFence {
    int Behavior();
};

int ChainChompFence::Behavior()
{
    void * c = (void *)this;
if (*(unsigned char*)((char*)c+0x31e) != 0) return 1;
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    return 1;

}
