//cpp
extern "C" {
extern void _ZN4Heap7DestroyEv(void);
}

struct Heap {
    void _Destroy(void);
};

void Heap::_Destroy(void)
{
_ZN4Heap7DestroyEv();

}
