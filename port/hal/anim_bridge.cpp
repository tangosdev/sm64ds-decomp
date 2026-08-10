// Gate-7-only bridge: the C reference to Animation::SetFlags forwards to
// the MSVC method definition. Lives apart from gx_upload_bridge because
// only targets that carry the gate-7 slice have the method to forward to.
struct Animation {
    void SetFlags(int flags);
};
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN9Animation8SetFlagsEi(void *self, int flags)
{
    ((Animation *)self)->SetFlags(flags);
}
#else
extern "C" void _ZN9Animation8SetFlagsEi(void *self, int flags);  /* Linux: real symbol from src/_ZN9Animation8SetFlagsEi */
#endif /* _WIN32 */
