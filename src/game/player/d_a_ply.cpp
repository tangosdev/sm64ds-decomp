//cpp
#include "daPly_c.h"
#include "EndingFlyPoint.h"

/* Reconstructed source-style name: SM64DS proves daPly_c through RTTI,
 * allocation size, vtable identity, and the PLAYER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Player_Spawn. */
extern "C" daPly_c *daPly_c_classInit() {
    return new daPly_c;
}

/* Super Mario 64's sJumboStarKeyframes: the jumbo star flight's path.
   After the take-off it circles the origin at radius ~4000 for two laps,
   climbing from y 713 to 2300. AnimSplinePoll blends four consecutive rows
   and advances by speed / 1000; the three speed-0 rows end the flight. The
   N64 table is the same but for its first row, {20, 0, 678, -2916}. */
EndingFlyPoint gJumboStarKeyframes[27] = {
    {20, 0, 713, -2970},      {30, 0, 680, -3500},      {40, 1000, 700, -4000},
    {50, 2500, 750, -3500},   {50, 3500, 800, -2000},   {50, 4000, 850, 0},
    {50, 3500, 900, 2000},    {50, 2000, 950, 3500},    {50, 0, 1000, 4000},
    {50, -2000, 1050, 3500},  {50, -3500, 1100, 2000},  {50, -4000, 1150, 0},
    {50, -3500, 1200, -2000}, {50, -2000, 1250, -3500}, {50, 0, 1300, -4000},
    {50, 2000, 1350, -3500},  {50, 3500, 1400, -2000},  {50, 4000, 1450, 0},
    {50, 3500, 1500, 2000},   {50, 2000, 1600, 3500},   {50, 0, 1700, 4000},
    {50, -2000, 1800, 3500},  {50, -3500, 1900, 2000},  {30, -4000, 2000, 0},
    {0, -3500, 2100, -2000},  {0, -2000, 2200, -3500},  {0, 0, 2300, -4000},
};
