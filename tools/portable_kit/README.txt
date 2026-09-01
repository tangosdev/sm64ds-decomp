Super Mario 64 DS - PC port
===========================

This is a work-in-progress PC build of Super Mario 64 DS, made from a
decompilation of the game. It is the real game code running natively on your
PC: the real physics, the real camera, the real Mario.

It contains no game data at all. No models, no textures, no music, no levels.
All of that comes out of a Super Mario 64 DS cartridge that you own.


WHAT YOU NEED
-------------

1. A Windows PC (Windows 10 or 11). Nothing to install.

2. Your own copy of Super Mario 64 DS for the Nintendo DS, and a dump of that
   cartridge saved as a .nds file. Dumping a cartridge you own is done with a
   DS and a dumping tool; how to do that is outside this kit. A copy
   downloaded from the internet is not the same thing and is not supported
   here.

   European and North American cartridges both work.


HOW TO PLAY
-----------

1. Copy your .nds file into the folder named PLACE YOUR ROM HERE.

2. Double-click SM64DSLauncher.exe and press Play.

   The first time you do this it unpacks the game data out of your dump. That
   takes about a minute and creates two folders here, "extracted" and
   "build". It only happens once; after that Play starts the game straight
   away.

3. That is all.


PLAYING WITH OTHER PEOPLE
-------------------------

You can play a versus match against other people over the internet. Nobody
needs to change any setting on their router.

1. Open the launcher and click Multiplayer.

2. Type a name for yourself. Then either click Make a room, or type in the
   six-letter code a friend read out to you and click Join.

3. Whoever made the room is the host. Everyone else joining sees the room
   fill up. There is a chat box if you want to talk first.

4. The host picks the arena and how the match is won, either most stars when
   the clock runs out or first to a number of stars. Then the host clicks
   Start and everybody's game opens.

5. When the match ends the game says who won and everyone comes back to the
   room, so the host can start another one straight away.

The room code is six letters and is not case sensitive. A room disappears
once everybody has left it, so a code is only good while somebody is still in
there. The host can remove somebody from the room.

Nothing about this needs an account and nothing is saved anywhere. Your name
and your chat are only ever held in memory on the server while the room is
open, and they go when the room does.


CONTROLS
--------

Xbox controller:

    Left stick          walk
    A                   jump
    X                   run
    B                   punch
    Right trigger       crouch
    Right stick         swing the camera around
    Bumpers             zoom the camera in and out
    Right stick click   same as F1 below

Keyboard and mouse:

    W A S D or arrows   walk
    Space               jump
    Shift               run
    Ctrl                crouch
    X                   punch
    Q and E             swing the camera around
    R and F             tilt the camera
    C                   put the camera back behind Mario
    Right mouse drag    look around
    Mouse wheel         zoom
    Esc                 open or close the debug menu (quit with Alt+F4
                        or the window's X button)

Extra keys:

    F1   change camera: chase rig, free camera, DS-exact stepped rotate
    F3   stats overlay (frame rate, where Mario is, what state he is in)
    F4   switch character on the spot: Mario, Luigi, Wario, Yoshi
    F5   debug menu (same as Esc). Arrows or the d-pad move, Enter or A
         picks, F5 or Esc closes.
         The game pauses while it is open. Rows:
             warp to any entrance in the level
             level select (the game's own table; picking a level loads it)
             exit course
             character
         F5 does nothing during a multiplayer match, on purpose.
    Tab  show or hide the DS bottom screen under the main view


WHAT YOU CAN DO IN IT
---------------------

The game boots to its own title screen. Every level in the game is mounted
and reachable from the level select, most of them populated with their real
enemies, items and music, and the minigames are playable. In a versus match
each player is a different coloured Yoshi, the timer and star counts show on
screen, and the match ends by naming the winner.

This is a port in progress rather than a finished game. Some things are
missing or rough, and some levels are further along than others.


IF SOMETHING GOES WRONG
-----------------------

"No .nds file found"
    Your dump is not here yet. Put it in the PLACE YOUR ROM HERE folder. Its
    name has to end in .nds.

"That is not a Super Mario 64 DS dump"
    The .nds file here is a different game, or the dump is damaged.

"This dump is truncated" or "incomplete"
    The dump did not finish. Dump the cartridge again.

"...but not a revision this build knows"
    The cartridge is a Super Mario 64 DS release this build has not been
    matched against. Nothing to be done from here.

The game window opens and closes immediately
    Start it from the launcher rather than the exe so the message stays on
    screen. If it keeps happening, the launcher's crash report is the fastest
    way for it to get fixed.


WHAT IS IN THIS FOLDER
----------------------

    SM64DSLauncher.exe     the launcher: Play, Multiplayer, settings, updates
    walk_window.exe        the game itself (the launcher starts this)
    PLACE YOUR ROM HERE\   where your .nds dump goes
    README.txt             this file

Once you have run it, two more folders appear, both made from your own dump:

    extracted\           the game's files, exactly as your cartridge holds them
    build\assets\        the small index and table files that name them, plus
                         the game's own data tables rebuilt from your dump

The game itself reads your dump, writes those two folders next to itself, and
runs. The launcher checks tangos.dev for updates, and if the game crashes it
can send a small crash report so the bug gets fixed; that can be turned off
in the launcher's settings. Multiplayer talks to our server only while you
are in a room or a match. Nothing else here touches the internet or changes
any setting on your PC.
