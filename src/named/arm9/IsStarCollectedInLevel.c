/* IsStarCollectedInLevel at 0x020136f8
 * Converts a sublevel ID to a course ID, then checks if a star is collected in that course.
 */
typedef signed char s8;
typedef int s32;

extern s8 SublevelToLevel(s8 levelID);
extern int IsStarCollected(s8 courseID, s32 starID);

int IsStarCollectedInLevel(s8 levelID, s32 starID)
{
    s8 courseID = SublevelToLevel(levelID);
    return IsStarCollected(courseID, starID);
}
