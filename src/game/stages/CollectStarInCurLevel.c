/* CollectStarInCurLevel at 0x0202a6ac
 * Collects a star in the current level.
 */
typedef signed char s8;
typedef int s32;

extern s8 data_0209f2f8; /* current level ID */
extern void CollectStarInLevel(s8 levelID, s32 starID);

void CollectStarInCurLevel(s32 starID)
{
    CollectStarInLevel(data_0209f2f8, starID);
}
