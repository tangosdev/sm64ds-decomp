#ifndef ENDINGFLYPOINT_H
#define ENDINGFLYPOINT_H

/* One row of gJumboStarKeyframes (Super Mario 64 kept these as Vec4s): the
   speed at which the spline advances past this point, in thousandths of a
   span per frame, then its position. */
struct EndingFlyPoint {
    short speed;
    short x;
    short y;
    short z;
};

#endif
