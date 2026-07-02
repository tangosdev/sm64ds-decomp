extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void func_ov098_02138ce0(void *);
extern void func_ov098_02139850(void *);
extern void func_ov098_021397c8(void *);
void func_ov098_021381e8(char *c)
{
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124))
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    func_ov098_02138ce0(c);
    func_ov098_02139850(c);
    func_ov098_021397c8(c);
    (*(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF)) &= ~0xe0000;
}
