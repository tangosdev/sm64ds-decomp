extern void func_02049ee8(void* a, void* b);
extern char* PARTICLE_SYS_TRACKER;
extern int VIEW_MATRIX_ASR_3;

void _ZN8Particle9RenderAllEv(void)
{
    char* p = PARTICLE_SYS_TRACKER;
    if (p == 0) return;
    func_02049ee8(*(void**)(p + 4), &VIEW_MATRIX_ASR_3);
}
