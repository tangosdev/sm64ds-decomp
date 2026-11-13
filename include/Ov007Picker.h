#ifndef OV007_PICKER_H
#define OV007_PICKER_H

#include "types.h"
#include "Ov007StrokePath.h"
#include "Ov007Spring.h"

/* Reconstructed names and layout, not an original class identity. */

/* The overlay's 0xd8-byte camera (func_ov007_020c023c), defined in
 * Ov007Camera.h. Only a pointer is held here, so the tag stays incomplete
 * and Picker consumers are not made to see math/Matrix.h's Matrix4x3. */
struct Ov007Camera;

/* The 0x20-byte picker parameters, data_ov007_020d7b60
 * (PickerScene_Create passes it to func_ov007_020c690c). */
typedef struct Ov007PickerParams {
    int pointMass; /* 0x00: Q12, each group's points (func_ov007_020c798c) */
    int damping;   /* 0x04: func_ov007_020c81a0's damping for every point */
    int minLen;    /* 0x08: pixels between appended points (func_ov007_020c65bc) */
    int maxLen;    /* 0x0c */
    int minTurn;   /* 0x10: degrees; a gentler turn drops the point */
    int word14;    /* 0x14: no reader found */
    /* 0x18: degrees; func_ov007_020c281c drops each point whose segment
     * turns by less than it (func_ov007_020c2dfc), and a negative value
     * keeps every point (func_ov007_020c6d20) */
    int mergeAngle;
    /* 0x1c: Q12; a group closes when its ends are within its length times
     * this (func_ov007_020c6e68) */
    int closeFactor;
} Ov007PickerParams;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007PickerParams_size_must_be_0x20[sizeof(Ov007PickerParams) == 0x20 ? 1 : -1];
#endif

/* The 0xd0-byte touch picker that func_ov007_020c690c allocates and
 * func_ov007_020c684c resets. The player draws a stroke into `path`. Each
 * committed stroke becomes a group: a point list joined by spring links,
 * which func_ov007_020c5188 steps toward `pos`. */
typedef struct Ov007Picker {
    int word00; /* 0x00: no reader found */
    int mode;   /* 0x04: func_ov007_020c5a6c switches on it (0 move, 1 drag, 2 link) */
    /* 0x08: committed strokes. func_ov007_020c6550 commits the stroke as
     * group groupCount and counts it, func_ov007_020c6724 clears it */
    int groupCount;
    int maxGroups; /* 0x0c: func_ov007_020c6550 stops committing at it */
    int maxPoints; /* 0x10: func_ov007_020c65bc stops appending at it */
    /* 0x14, medium confidence: func_ov007_020c6784 seeds it with
     * func_ov007_020c5db0's point total over the stroke and every group. Undo
     * and redo take a group's capacity out or put it back */
    int totalPoints;
    /* 0x18: three (stiffness, damping) pairs that func_ov007_020c5188 hands
     * to the spring steps: [0..1] func_ov007_020c7560 per point list, [2..3]
     * func_ov007_020c7b2c per groupLinks entry, [4..5] the same for
     * headLinks/tailLinks. One six-int row of data_ov007_020d7b80
     * (PickerScene_UpdatePicker) */
    int *springConstants;
    Ov007PickerParams *params; /* 0x1c */
    Ov007StrokePath *path;     /* 0x20: the stroke being drawn (func_ov007_020c2c68) */
    /* 0x24: the path's points; func_ov007_020c65bc unprojects each appended
     * point into it through `camera` */
    Vector3 *pathPoints;
    /* 0x28: per group, the points unprojected through camera
     * (func_ov007_020c6e68); the springs move them */
    Vector3 **groupPoints;
    Vector3 **groupPointCopies;  /* 0x2c: per group, the rest copy of groupPoints */
    int word30;                  /* 0x30: func_ov007_020c690c clears it; no reader found */
    Ov007Link ***groupLinks;     /* 0x34: per group, one link per point from posPoint */
    Ov007StrokePath **groups;    /* 0x38: committed strokes (func_ov007_020c6d20) */
    Ov007PointList **pointLists; /* 0x3c: per group (func_ov007_020c798c) */
    /* 0x40: per group, the link func_ov007_020c6e68 appends after the
     * per-point ones; func_ov007_020c6174 measures it from the group's first
     * point to groupPointCopies[headLinkGroups[i]][headLinkPoints[i]] */
    Ov007Link **headLinks;
    Ov007Link **tailLinks;   /* 0x44: likewise from the group's last point */
    u16 *headLinkGroups;     /* 0x48: per group, 0xffff = unset */
    u16 *tailLinkGroups;     /* 0x4c */
    u16 *headLinkPoints;     /* 0x50 */
    u16 *tailLinkPoints;     /* 0x54 */
    int hitGroup;            /* 0x58: picked group, -1 = none */
    int hitIndex;            /* 0x5c: picked point in that group, -1 = none */
    Vector3 hitOffset;       /* 0x60 */
    Ov007Follower *posPoint; /* 0x6c: func_ov007_020c844c's follower over pos */
    Vector3 pos;             /* 0x70 */
    /* 0x7c: func_ov007_020c6af0 snapshots pos here; func_ov007_020c5dec
     * copies it back, func_ov007_020c6314 measures rest link lengths from it */
    Vector3 restPos;
    /* 0x88: func_ov007_020c5188 adds it to pos for one simulation step and
     * then restores pos; PickerScene_UpdateMoveAnim sweeps it per menu state */
    Vector3 posOffset;
    /* 0x94: func_ov007_020c844c's follower over anchorPos; `link`'s from end */
    Ov007Follower *anchorPoint;
    /* 0x98: func_ov007_020c64c4 copies pos into it; mode 2 of
     * func_ov007_020c5a6c moves it to the hit point */
    Vector3 anchorPos;
    Ov007Link *link;            /* 0xa4: func_ov007_020c80a4; from anchorPoint to posPoint */
    struct Ov007Camera *camera; /* 0xa8: func_ov007_020c65bc unprojects through it */
    /* 0xac/0xb0: the draw limit. func_ov007_020c4dfc draws whole groups
     * before limitGroup, then that group's first limitIndex points;
     * func_ov007_020c5a6c's flagged picks stop before it */
    int limitGroup;
    int limitIndex;
    s16 limitFraction; /* 0xb4: Q12 progress toward point limitIndex (func_ov007_020c5014) */
    /* 0xb8/0xc4: the screen rectangle func_ov007_020c510c unprojects; pos
     * (func_ov007_020c5948) and every point (func_ov007_020c5188) are clamped
     * to it on x and y */
    Vector3 boundsMin;
    Vector3 boundsMax;
#ifdef __cplusplus
    int func_ov007_020c5014(int x);
    void func_ov007_020c510c();
#endif
} Ov007Picker;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Picker_size_must_be_0xd0[sizeof(Ov007Picker) == 0xd0 ? 1 : -1];
typedef char Ov007Picker_camera_must_be_at_0xa8[(int)&((Ov007Picker *)0)->camera == 0xa8 ? 1 : -1];
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

struct Ov007Camera;

void func_ov007_020c4dfc(Ov007Picker *picker, int half, int a2, int a3);
int func_ov007_020c5854(Ov007Picker *picker, Vector3 *target, int pull, int snapRadius, int reach,
                        int scale);
void func_ov007_020c5948(Ov007Picker *picker, Vector3 *src);
void func_ov007_020c5fc8(Ov007Picker *picker, int idx, int t);
void func_ov007_020c6174(Ov007Picker *picker, int idx, int flag);
void func_ov007_020c62cc(Ov007Picker *picker, int flag);
void func_ov007_020c6314(Ov007Picker *picker, int idx, int flag);
void func_ov007_020c64c4(Ov007Picker *picker, int flag);
int func_ov007_020c6550(Ov007Picker *picker);
int func_ov007_020c65bc(Ov007Picker *picker, int x, int y, int progress);
void func_ov007_020c6684(Ov007Picker *picker);
void func_ov007_020c6724(Ov007Picker *picker, int flag);
void func_ov007_020c6d20(Ov007Picker *picker, int idx);
void func_ov007_020c6d90(Ov007Picker *picker, int idx);
void func_ov007_020c6e68(Ov007Picker *picker, int idx);
void func_ov007_020c6784(Ov007Picker *picker, int count, const int *pointCounts,
                         const void *xCoords, const void *yCoords,
                         const int *const *progressThresholds, int mode);
Ov007Picker *func_ov007_020c690c(int maxGroups, int maxPoints, int pathCapacity,
                                 struct Ov007Camera *camera, void *params, void *springConstants);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif
