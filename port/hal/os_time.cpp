// Host implementation of the system tick, func_02059650.
//
// On the DS this reads the OS tick counter (timer hardware + IRQ-maintained
// high bits). Timer::Start/Stop/GetTime take differences of it, so any
// monotonic s64 with a stable rate is semantically faithful. The Timer TUs
// used to declare it with C++ linkage, so this file was a .cpp providing the
// mangled spelling; since the shared decl headers were wrapped in extern "C"
// (#1049) they reference the plain C symbol, so the definition carries C
// linkage and the file stays .cpp only for the s64 typedef and the hook.
//
// GATE 1: the tick is a manually-advanced counter so the smoke tests are
// deterministic -- sm64ds_hal_advance_ticks() stands in for time passing.
// When the main-loop seam lands, this becomes a QueryPerformanceCounter read
// scaled to the DS tick rate, and the manual hook moves behind a test flag.
typedef long long s64;

static s64 g_ticks;

extern "C" s64 func_02059650()
{
    return g_ticks;
}

extern "C" void sm64ds_hal_advance_ticks(s64 n)
{
    g_ticks += n;
}
