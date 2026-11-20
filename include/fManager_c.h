#ifndef FMANAGER_C_H
#define FMANAGER_C_H

#include "ProcessList.h"

namespace ProcessTree {
struct Thing;
}

/* The process manager: the frame loop and the process tree and lists it
 * walks. The class and member names are NSMBW-decomp's
 * (its f_manager.hpp and f_manager.cpp),
 * and the ROM matches that file on every point checked:
 *
 *   - each manager is built from NSMBW's callback. The member-pointer
 *     constants are 0x02099f60 connectProc, 0x02099f70 createPack,
 *     0x02099f48 executePack, 0x02099f68 drawPack and 0x02099f50 deletePack;
 *   - src/fListNode.cpp defines them in NSMBW's definition order (connect,
 *     create, execute, draw, delete), which __sinit_fListNode.cpp follows;
 *   - mainLoop walks delete, create, execute, draw, then connect, NSMBW's
 *     order, storing each phase's LOOP_PROC_e in m_nowLoopProc and NONE at
 *     the end;
 *   - m_nowLoopProc's .data initial value is 1, NSMBW's `= CONNECT`.
 *
 * NSMBW's m_searchManage and m_StopProcInf have no counterpart here: this
 * mainLoop tests no stop flags, and no search lists have been found. The
 * manager types keep this tree's ProcessList and ProcessTree::Thing
 * (NSMBW's fLiMgBaFuPr_c and fTrMgBaFu_c).
 *
 * The executing process node (data_020a4b68; fListNode.cpp's destructors
 * clear it, and the crash screen reads it) is state NSMBW's fManager_c does
 * not have, so it keeps its address name and is not a member here.
 *
 * NSMBW keeps the statics private. They are public here because
 * ActorBase.cpp and ShowCrashScreen.cpp reach them directly. */
class fManager_c {
public:
    enum LOOP_PROC_e { NONE, CONNECT, CREATE, EXECUTE, DELETE, DRAW };

    static void mainLoop();

    static ProcessTree::Thing m_connectManage;
    static ProcessList m_createManage;
    static ProcessList m_executeManage;
    static ProcessList m_drawManage;
    static ProcessList m_deleteManage;
    static LOOP_PROC_e m_nowLoopProc;
};

#endif
