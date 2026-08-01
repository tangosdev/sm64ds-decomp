/* _Z19UnloadLevelOverlaysi at 0x0202de64 (72 bytes)
 * UnloadLevelOverlays(s32 levelID) -- unloads per-level overlay then object overlays.
 */

typedef int s32;

extern s32 data_02092130; /* 0x02092130 */

extern void UnloadOverlay(s32 ovID); /* 0x02017f34 */
extern void _Z26LoadOrUnloadObjectOverlaysPFviEi(void (*func)(s32), s32 levelID); /* 0x0202df70 */

void _Z19UnloadLevelOverlaysi(s32 levelID)
{
    if (data_02092130 != -1)
    {
        UnloadOverlay(data_02092130);
        data_02092130 = -1;
    }
    _Z26LoadOrUnloadObjectOverlaysPFviEi(UnloadOverlay, levelID);
}
