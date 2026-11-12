/* playlog_commit.cpp -- put the flight recorder on the DISK around the lobby restart (run coopfix1, lane HOSTSTART1, bug 26).

   WHAT WAS SEEN. Two of a player's six host starts on 1.0.2 "vanished": the host's playlog ends at "[lobby] preflight -> 200"
   and the guests hear nothing. Both starts went through THE RESTART (hal/lobby_client.cpp: a code mod was loaded, so the
   running game refused the in-place start and started a fresh copy of itself with the room). The logs say the processes did
   not crash:
     - the first host's perf report (playlog/perf-*.jsonl) ends with {"type":"end","reason":"exit"} at the very second the
       successor's playlog was created, so that process ran its whole restart and exited cleanly;
     - yet ITS playlog lost everything after preflight: the file is 8356 bytes longer than its text and those bytes are NUL;
     - the successor's playlog stops after 576 bytes in the MIDDLE of the "[cpu] hybrid" line, which is ONE fprintf on an
       unbuffered stderr (one WriteFile), and its perf report is 0 bytes;
     - the guest saw the successor's seat join the hub and go silent 3 s later.
   Text a process has handed to WriteFile survives that process crashing or being killed: Windows still writes it out. Bytes
   that come back as NUL past the text, and a single write cut in half, are what the disk holds when the MACHINE stops (power,
   a hard freeze, a driver crash) before the system cache was written out. Both times the player's 7-minute gap to the next
   log fits a reboot.

   WHAT THIS DOES. The recorder is unbuffered but lives in the system cache, which a machine stop loses. Around the restart
   the lines are now committed to the disk itself (FlushFileBuffers), so the next time this happens the log says how far the
   restart got instead of stopping at preflight:
     - port_playlog_commit(): one commit, called by the restart path right after "restarted as pid N" and at that process's
       exit;
     - port_playlog_successor_boot(): called once where walk_window opens the recorder; in a process started BY the restart
       (SM64DS_LOBBY_RESUME is set) it says so in one line, names the old process and whether it is still running, and starts
       a small thread that commits the log every 250 ms for the first 30 s (the stretch that was lost).
   Nothing changes anywhere else: a game not started by the lobby restart never runs any of it, solo play included, and no
   game thread waits on the disk. Only a recorder that is a disk file is committed (a pipe or a console is left alone, since
   FlushFileBuffers on a pipe waits for the reader). */

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

namespace {

HANDLE recorder_handle(void)
{
    const int fd = _fileno(stderr);
    if (fd < 0) return INVALID_HANDLE_VALUE;
    const intptr_t h = _get_osfhandle(fd);
    if (h == -1 || h == -2) return INVALID_HANDLE_VALUE;
    if (GetFileType((HANDLE)h) != FILE_TYPE_DISK) return INVALID_HANDLE_VALUE;
    return (HANDLE)h;
}

DWORD WINAPI commit_thread(LPVOID arg)
{
    const DWORD span = (DWORD)(UINT_PTR)arg, t0 = GetTickCount();
    unsigned n = 0;
    while (GetTickCount() - t0 < span) {
        Sleep(250);
        const HANDLE h = recorder_handle();
        if (h != INVALID_HANDLE_VALUE && FlushFileBuffers(h)) ++n;
    }
    fprintf(stderr, "[recorder] restart window over: %u commit(s) to disk in %lu ms\n", n, (unsigned long)(GetTickCount() - t0));
    const HANDLE h = recorder_handle();
    if (h != INVALID_HANDLE_VALUE) FlushFileBuffers(h);
    return 0;
}

}  // namespace

extern "C" void port_playlog_commit(void)
{
    fflush(stderr);
    const HANDLE h = recorder_handle();
    if (h != INVALID_HANDLE_VALUE) FlushFileBuffers(h);
}

extern "C" void port_playlog_successor_boot(void)
{
    const char *resume = getenv("SM64DS_LOBBY_RESUME");
    if (!resume || !*resume) return;
    const char *pp = getenv("SM64DS_LOBBY_PREDECESSOR");
    const unsigned long pid = pp ? strtoul(pp, 0, 10) : 0;
    const char *state = "not named (an older build started this one)";
    if (pid) {
        HANDLE p = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
        state = !p ? "gone" : WaitForSingleObject(p, 0) == WAIT_OBJECT_0 ? "gone" : "still running";
        if (p) CloseHandle(p);
    }
    if (pp) _putenv_s("SM64DS_LOBBY_PREDECESSOR", "");
    const bool disk = recorder_handle() != INVALID_HANDLE_VALUE;
    fprintf(stderr, "[recorder] started by the lobby restart; the old game (pid %lu) is %s; %s\n", pid, state,
            disk ? "this log is committed to disk every 250 ms for the first 30 s" : "the recorder is not a disk file, nothing to commit");
    if (!disk) return;
    port_playlog_commit();
    if (HANDLE t = CreateThread(0, 0, commit_thread, (LPVOID)(UINT_PTR)30000, 0, 0)) CloseHandle(t);
    atexit(port_playlog_commit);
}
