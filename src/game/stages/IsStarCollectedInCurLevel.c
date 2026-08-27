/* IsStarCollectedInCurLevel at 0x0202a6c8
 * Checks whether a star is collected in the current level.
 */
typedef signed char s8;
typedef int s32;

extern s8 data_0209f2f8; /* current level ID */
extern s32 IsStarCollectedInLevel(s8 levelID, s32 starID);

s32 IsStarCollectedInCurLevel(s32 starID)
{
    return IsStarCollectedInLevel(data_0209f2f8, starID);
}
