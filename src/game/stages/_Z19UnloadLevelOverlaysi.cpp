//cpp
// @symbol _Z19UnloadLevelOverlaysi
//
// Language-mode flip only: the compiler mangles UnloadLevelOverlays(int), it is
// no longer spelled by hand. Body preserved; free function (not a method).
// Sibling: src/_Z17LoadLevelOverlaysi.cpp.
/* UnloadLevelOverlays(int levelID) at 0x0202de64 (72 bytes)
 * Unloads the per-level overlay (if any) then the object overlays. */

extern "C" {
extern int data_02092130; /* 0x02092130 */
extern void UnloadOverlay(int ovID); /* 0x02017f34 */
}

void LoadOrUnloadObjectOverlays(void (*fn)(int), int levelID); /* 0x0202df70 */

void UnloadLevelOverlays(int levelID)
{
    if (data_02092130 != -1)
    {
        UnloadOverlay(data_02092130);
        data_02092130 = -1;
    }
    LoadOrUnloadObjectOverlays(UnloadOverlay, levelID);
}
