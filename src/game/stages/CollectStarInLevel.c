/* CollectStarInLevel at 0x020136dc
 * Converts a sublevel ID to a course ID, then collects a star in that course.
 */
typedef signed char s8;
typedef int s32;

extern s8 SublevelToLevel(s8 levelID);
extern void CollectStar(s8 courseID, s32 starID);

void CollectStarInLevel(s8 levelID, s32 starID)
{
    s8 courseID = SublevelToLevel(levelID);
    CollectStar(courseID, starID);
}
