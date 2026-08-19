// @symbol _ZN21ClockPaintingPendulum16CleanupResourcesEv
// recovered name: ClockPaintingPendulum::CleanupResources
/* recovered: renamed to Class_Method, vtable slot 3 */
/* ClockPaintingPendulum::CleanupResources -- vtable slot 3, ov013
 * 0x02111214. Same idiom as src/_ZN4Door13InitResourcesEv.c: declared as an
 * override in include/ClockPaintingPendulum.h, defined here under the
 * mangled symbol (a plain C file emits it verbatim, no extern "C" needed),
 * not as a real ClockPaintingPendulum:: method. */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov013_02112280[];
int _ZN21ClockPaintingPendulum16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov013_02112280);
    return 1;
}
