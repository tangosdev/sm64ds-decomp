//cpp
extern "C" {

struct Animation { void Advance(); int Finished(); };
struct fBase_c { void MarkForDestruction(); };
extern void func_ov002_020e7fcc(void *);

void func_ov002_020e9804(char *thiz)
{
    ((Animation *)(thiz + 0x35c))->Advance();
    func_ov002_020e7fcc(thiz);
    if (!((Animation *)(thiz + 0x35c))->Finished()) return;
    ((fBase_c *)thiz)->MarkForDestruction();
}

}
