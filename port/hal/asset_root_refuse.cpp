// THE KIT BUILD HAS NO ASSET-ROOT FALLBACK, AND THAT IS THE WHOLE POINT.
//
// Run link60, lane KIT1. port/kit_pipeline.txt is the write-up; this is the
// mechanism it describes.
//
// Every seam that opens a data file resolves it under SM64DS_ASSET_ROOT, and
// every one of them used to end the same way:
//
//     const char *env = getenv("SM64DS_ASSET_ROOT");
//     return env ? env : PORT_REPO_ROOT;
//
// PORT_REPO_ROOT is an absolute path baked in by cmake at configure time. On
// the machine that built the exe it names a checkout with a complete
// build/assets in it. On a player's machine it names a directory that does not
// exist, so the fallback is harmless there -- and that is exactly what made it
// dangerous. THE FALLBACK IS INVISIBLE ON EVERY MACHINE THAT CAN TEST IT AND
// FATAL ON EVERY MACHINE THAT CANNOT.
//
// What it cost, concretely. The open-by-name seam (hal/fs_names.cpp) started
// requiring build/assets/nitrofs.tsv and the two table copies next to it. The
// kit's extractor was never taught to write them, so every fresh player
// extraction was missing three files the game refuses to start without. Nobody
// saw it. Every developer run -- every battery row, every selftest, every
// packaged-kit run done from a checkout -- had SM64DS_ASSET_ROOT pointed at a
// tree that had them, or fell through to PORT_REPO_ROOT and found the same
// tree anyway. The first machine to disagree was the project owner's, playing
// the shipped zip, and it died in about a fifth of a second.
//
// So in a PORT_ROM_CLEAN build -- the build the kit ships -- there is no
// fallback at all. An unset SM64DS_ASSET_ROOT is a refusal that names the file
// it was about to look for, writes startup_error.txt for the launcher to show,
// and exits. A kit exe cannot read a developer's repo tree because it no
// longer knows how to look at one.
//
// The DEVELOPER build is untouched, deliberately. Every lane in this tree runs
// walk_window straight out of a build directory with no environment set and
// expects PORT_REPO_ROOT to find the catalog. That convenience is correct for
// a checkout and wrong for a zip, and PORT_ROM_CLEAN is exactly the line
// between the two: it is already the flag that means "this binary ships".
//
// This file is compiled ONLY into the PORT_ROM_CLEAN targets (see the
// if(PORT_ROM_CLEAN) blocks in port/CMakeLists.txt), and the call sites are
// inside #ifdef PORT_ROM_CLEAN, so the developer build's translation units come
// out of the preprocessor exactly as they did before this lane.
//
// ---- WHY THE FILE WRITE IS DUPLICATED HERE ---------------------------------
//
// tests/walk_window.cpp has port_startup_error_write() and it is the same
// twenty lines. It is static, and more to the point it runs inside main().
// hal/fs_names.cpp's table load is reached from a STATIC INITIALISER, before
// main has run, which is the reason its own failure path calls exit() and says
// so at length. A refusal that can fire before main cannot borrow a helper
// that lives inside it, so it carries its own. Raw Win32 and fixed buffers,
// the same discipline crash.txt and startup_error.txt already keep, because
// this path runs when the process is known to be in trouble.

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

namespace {

void refuse_path(char *path, unsigned cap)
{
    DWORD n = GetModuleFileNameA(0, path, cap);
    while (n && path[n - 1] != 92 /* '\\' */)
        --n;
    lstrcpynA(path + n, "startup_error.txt", (int)(cap - n));
}

/* Write `text` to startup_error.txt next to the exe and, unless this is an
   automated run, put it in a message box. Shared by both refusals below. */
void say_text(const char *text)
{
    char path[MAX_PATH + 32];
    HANDLE f;

    refuse_path(path, sizeof path);
    f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, 0);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(f, text, (DWORD)lstrlenA(text), &wr, 0);
        FlushFileBuffers(f);
        CloseHandle(f);
    }

    /* Same suppression rule tests/walk_window.cpp's dialog keeps: an automated
       run must never block on a modal box nobody is there to dismiss. */
    if (!getenv("SM64DS_NO_DIALOG") && !getenv("SM64DS_WINDOW_SELFTEST")) {
        HMODULE u32 = LoadLibraryA("user32.dll");
        if (u32) {
            int (WINAPI *mb)(HWND, LPCSTR, LPCSTR, UINT) =
                (int(WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))
                GetProcAddress(u32, "MessageBoxA");
            if (mb)
                mb(0, text, "Super Mario 64 DS could not start", 0x10);
        }
    }
}

} // namespace

/* Called from the asset_root() of every seam that resolves a data file, and
   only when SM64DS_ASSET_ROOT is absent. Never returns.

   exit(2) rather than abort(), for the reason hal/fs_names.cpp records at
   length: MSVC's abort() raises STATUS_STACK_BUFFER_OVERRUN, the shell reports
   0xC0000409, and every crash reporter in the chain then files a configuration
   error as a memory-safety crash in the port. 2 is the same code the other
   refusals on this path use. */
extern "C" void port_asset_root_refuse(const char *wanted)
{
    fprintf(stderr,
            "FATAL: SM64DS_ASSET_ROOT is not set and this build has no "
            "fallback.\n"
            "       It was about to open: <asset root>/%s\n"
            "       This is a shipping build. It reads its data only from the "
            "folder it\n"
            "       is told to; it will not fall back to a path baked in at "
            "build time.\n"
            "       Start it through play.bat or the launcher, which set that "
            "folder.\n", wanted);
    fflush(stderr);
    {
        char text[1024];
        /* The launcher shows this verbatim in a window, so it is a sentence a
           player can act on rather than a variable name. It still names the
           file, because the person who has to fix a broken install is
           sometimes us. */
        snprintf(text, sizeof text,
                 "Super Mario 64 DS could not start because it was not told "
                 "where its game data is.\r\n\r\n"
                 "Start the game with play.bat (or the launcher) instead of "
                 "running the .exe on its own. Those set the folder the game "
                 "reads its data from; the .exe by itself has no default and "
                 "will not guess one.\r\n\r\n"
                 "It was looking for: %s\r\n", wanted);
        say_text(text);
    }
    exit(2);
}

/* THE OTHER WAY AN ASSET ROOT IS WRONG: it exists, it is complete, and it
   belongs to a DIFFERENT BUILD OF THE GAME.

   Called from hal/romdata_loader.cpp when the romdata.bin in the asset root is
   not the one this exe's baked-in slice offsets were computed from. Its long
   write-up is at the top of that file; the short version is that an old install
   folder carries an old romdata.bin AND the old manifest that matches it, so
   every existing check passes and the exe then reads every ROM table out of the
   wrong bytes -- silently, and then fatally a few seconds later. No player has
   reported this yet; the hole was found by reading the loader, and
   port/tools/install_mismatch_probe.py demonstrates it on demand.

   THE WHOLE POINT IS THAT THIS IS NOT A CRASH. The player is told, in a
   sentence, that the folder and the game do not go together and what to do
   about it. Same exit(2) as the refusal above, and for the same recorded
   reason: abort() would be reported as 0xC0000409 and filed as a memory-safety
   bug in the port rather than the configuration error it is. */
extern "C" void port_asset_mismatch_refuse(const char *detail)
{
    char text[1400];
    snprintf(text, sizeof text,
             "Super Mario 64 DS could not start because the game data in this "
             "folder belongs to a different version of the game.\r\n\r\n"
             "This usually means the game is pointed at an older install "
             "folder. Reinstall or re-point it at the folder that came with "
             "THIS version, and delete the old one so it cannot be picked "
             "again.\r\n\r\n"
             "Loading it anyway would not work: the game would read its data "
             "from the wrong places and fail a few seconds later, which is what "
             "this message is here to prevent.\r\n\r\n"
             "The game data it found:\r\n"
             "%s\r\n", detail ? detail : "  (no detail)");
    say_text(text);
    exit(2);
}
