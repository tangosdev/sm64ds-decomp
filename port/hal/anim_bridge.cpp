// Gate-7-only bridge: the C reference to Animation::SetFlags forwards to
// the MSVC method definition. Lives apart from gx_upload_bridge because
// only targets that carry the gate-7 slice have the method to forward to.
struct Animation {
    void SetFlags(int flags);
};
extern "C" void _ZN9Animation8SetFlagsEi(void *self, int flags)
{
    ((Animation *)self)->SetFlags(flags);
}
