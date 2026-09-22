#ifndef PORT_HAL_DTORCALL_CENSUS_H
#define PORT_HAL_DTORCALL_CENSUS_H

/* A COUNT OF THE ACTOR TEARDOWNS THAT ACTUALLY REACH THE ACTOR'S OWN CLEANUP.
 * Run link100, lane DTORCALL.
 *
 * This exists because the defect it measures reported NOTHING. Between
 * 9cbd99048 and this lane, `this->~fBase_c()` in
 * src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp bound to a non-virtual
 * declaration, so every actor's teardown ran fBase_c's own destructor body and
 * no derived destructor was ever reached. No guard fired, no log line
 * appeared, no fault was raised, and the boot got FURTHER than it had before,
 * because the call that used to crash was no longer being made. A silent skip
 * that looks like a fix is the worst shape a regression can have, so the
 * dispatch is now counted rather than assumed.
 *
 * The counter is bumped by port_d16_face in port/hal/port_d16.h, which is the
 * wrapper every slot-16 store in the tree goes through (port/tools/
 * slot16_guard.py refuses any store that is not one). So the count is exactly
 * the number of times an actor's own complete-object destructor was dispatched
 * through ROM vtable slot 16, which is what the cartridge does at
 * 0x02043b84.
 *
 * Cost when quiet: one increment and one already-resolved branch, on a
 * teardown path. Printing is off unless SM64DS_DTORCALL_CENSUS is set to
 * something other than 0, so a normal run's log is byte-identical to one built
 * without this file.
 */

#include <cstdio>
#include <cstdlib>

/* C++17 inline variables, so this stays header-only and needs no new
   translation unit and no CMake source-list row. */
inline unsigned long port_dtorcall_dispatches = 0;
inline int port_dtorcall_verbose = -1;

inline void port_dtorcall_note(void *self)
{
    ++port_dtorcall_dispatches;
    if (port_dtorcall_verbose < 0) {
        const char *e = std::getenv("SM64DS_DTORCALL_CENSUS");
        port_dtorcall_verbose = (e && *e && *e != '0') ? 1 : 0;
    }
    if (!port_dtorcall_verbose)
        return;
    /* The vptr is still the most-derived table here: the dispatch has been
       taken but the destructor body that resets it has not run yet. */
    void *table = self ? *reinterpret_cast<void **>(self) : nullptr;
    std::printf("[dtorcall] slot-16 dispatch #%lu  actor %p  table %p\n",
                port_dtorcall_dispatches, self, table);
    std::fflush(stdout);
}

#endif
