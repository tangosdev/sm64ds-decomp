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

   Only the European cartridge works for now. A North American or Japanese
   dump is a different build, and the launcher tells you so.


HOW TO PLAY
-----------

1. Copy your .nds file into the folder named PLACE YOUR ROM HERE.

2. Double-click SM64DSLauncher.exe and press Play.

   The first time you do this it unpacks the game data out of your dump. That
   takes about a minute and creates two folders here, "extracted" and
   "build". It only happens once; after that Play starts the game straight
   away.

3. The menu, HUD and map art comes from a mod called HD UI Textures. The
   launcher fetches it the first time you start it (this needs the internet
   once), so the Mods page shows it as installed and on. Without it the game
   still works, but the menus and HUD use the DS's own plainer art.

4. If you want the game to start without the launcher, tick Skip launcher in
   Settings. Hold Shift while it starts to open the launcher again.

5. That is all.


PLAYING WITH OTHER PEOPLE (open beta)
-------------------------------------

Multiplayer is an open beta in 0.9.3. Expect rough edges, and please tell us
what breaks. Nobody needs to change any setting on their router, and nothing
needs an account.

Everyone in a room has to run the same build. If someone is on another
version they are turned away, and the launcher says why.

1. Open the launcher and click Multiplayer. A window opens.

2. Type a name for yourself under Your name. My colours picks the colours of
   your Yoshi and its shoes.

3. Press Play adventure online, or make or join a room:

   Play adventure online   An always-on world for up to 32 players. You play
                           your own game and see the others as see-through
                           figures with their name and stars.
   Create a VS room        A match for stars, up to 16 on the new netcode. The host picks the arena, how
                           many play and the mode under Game settings (Play
                           the clock, First to this many stars, King of the
                           Star or Luigi Infection), then presses START
                           MATCH. A referee on the server decides stars,
                           hits, the score and the winner; the host can tick
                           Classic netcode instead. You can join a match
                           that is already running.
   Create party room       Up to 4 players. Friends share the stars they
                           collect. In the game, walk next to a friend and
                           press D-pad Up (or B on the keyboard) to invite,
                           the same key to join, and D-pad Down (or N) twice
                           to leave. The host sets the Room rules (bumping,
                           hits, teleport to friends, share stars).
   Create coop room        Up to 4 players in one shared world. Enemies,
                           bosses, blocks, coins and stars are shared, and
                           each player gets their own cap or coins from a
                           block. The host picks a new or an existing coop
                           save, and it is kept on the host's PC. The room
                           ends when the host leaves.

   In party and coop rooms everyone presses PLAY when they are ready, and
   can do it at any time.

   A room gives you a six letter code. A friend types it into the Room code
   box and presses Join. The code is not case sensitive and only works while
   somebody is still in the room. The host can remove a player with the
   Remove player button, and that player cannot come back to the room for a
   couple of minutes.

Chat: press T to type, Enter to send, Esc to cancel. Messages show over
heads. The Chat size setting changes the text size. A small ping readout
sits at the bottom right while you play online. Inside a room, the window
has its own chat box too.

Online, mods, custom characters and save states are turned off, so every
player sees the same game.

Privacy: your name and your chat are only held in memory on the server while
the room is open, and they go when the room does. The server writes no names,
no chat and no internet addresses to disk. It keeps a plain log of rooms
opening, filling and closing (room codes and seat numbers, no addresses), and
so does the relay that carries the game traffic. The coop save stays on the
host's PC and is never sent.

To report a problem while playing, press F6 (the launcher keeps running
behind the game). A small box asks what happened. If bug reports are off in
Settings it asks before it sends. Also useful: the newest play_*.log in the
playlog folder next to the game, from each player, with the room type and
who hosted.


CONTROLS
--------

Xbox controller:

    Left stick          walk
    A                   jump
    X                   run
    B                   punch
    Y                   first-person look (press again to go back)
    Right trigger       crouch
    Left trigger        put the camera back behind Mario
    Right stick         turn the camera (the harder you push, the faster)
    Bumpers             turn the DS camera one step left or right (in the
                        chase and free cameras they zoom out and in)
    Right stick click   same as F1 below
    Start               the game's own Start button (pause)

Keyboard and mouse:

    W A S D or arrows   walk
    Space               jump
    Shift               run
    Ctrl                crouch
    X                   punch
    Z                   first-person look (press again to go back)
    Q and E             turn the camera
    R and F             tilt the camera (chase and free cameras only)
    C                   put the camera back behind Mario
    Enter               the game's own Start button (pause)
    Right mouse drag    look around (chase and free cameras only)
    Mouse wheel         zoom (chase and free cameras only)
    Esc                 back out of the game's Settings page and the
                        character picker. With the Debug menu setting on it
                        also opens or closes the debug menu. It does not
                        quit: use Alt+F4 or the window's X button.

The camera starts as DS stepped, the cartridge's own camera. F1 below
changes it.

Extra keys:

    F1   change camera: DS stepped (the default), chase rig, free camera
    F3   stats overlay (frame rate, where Mario is, what state he is in)
    F4   switch character on the spot: Mario, Luigi, Wario, Yoshi (not while
         you are playing online)
    F5   debug menu (same as Esc), only when the Debug menu setting is on
         (the game's Settings, General page, off by default). Arrows or the
         d-pad move, Enter or A picks, F5 or Esc closes. The controller's
         Back button opens it too.
         The game pauses while it is open. Rows include:
             warp to any entrance in the level
             level select (the game's own table; picking a level loads it)
             minigames and the VS test maps
             exit course
             character
             camera, running and save state rows
         F5 does nothing during a multiplayer match, on purpose.
    F6   report a problem (needs the launcher; see Playing With Other People)
    Tab  show or hide the DS bottom screen under the main view
    F8   save a state (not during cutscenes, and not online)
    F9   load the saved state (it is not loaded automatically at startup)
    F11 or F12   borderless fullscreen on or off
    T    text chat, when you are playing online

Every camera action can be put on a key, a pad button or a mouse button
(Settings, Controls). The list has Rotate camera left and right, Tilt camera
up and down, Zoom camera in and out, and Center camera. Zoom is not bound to
anything until you pick a key. Right stick zooms makes the right stick zoom
the DS camera instead of turning it. Camera speed also changes how fast the
DS camera turns. The camera binds and sticks do nothing while the game is
paused.

A controller works in every menu: A confirms and B goes back. On the face
pages and in the minigames the right stick moves a small pointer, A taps
and Start pauses.

First-person look (Y and Z above) can be rebound like the other buttons,
under Controls in the launcher's Settings.

Frame rate is on the General page of the launcher's Settings. Native is the
DS's own speed. Pick 60 or more and the game draws extra pictures in
between, so motion looks smooth. It takes effect the next time the game
starts.


NEW SETTINGS IN 0.9.3
---------------------

These are in the launcher's Settings. The ones marked (game) are also on the
Settings page inside the game: open the pause menu's options, or press the
wrench on the title screen.

    Start fullscreen         General page. Starts the game in borderless
                             fullscreen. Off by default.

    Chat size (game)         General page. How big the text chat is in
                             online play, 75% to 200%. 100% by default.

    One-screen menus         General page, on by default. Directly under it:
    Coins in the middle      Moves the coin counter to the top middle, like
    (game)                   SM64. Off by default. Off keeps the DS layout.

    Custom characters from   General page. Lets you pick a custom character
    the start (game)         at the Rec Room's white door without unlocking
                             it first. Off by default. Needs a character mod.

    Character select on      General page. Shows a row of character heads
    star select (game)       before the course on the star select screen.
                             On by default. Off keeps the DS caps row.

    Skip launcher            General page. Starts the game straight away
                             after a short Starting window. Hold Shift while
                             it starts to open the launcher. Off by default.

    add to Steam             General page button. Adds the launcher to Steam
                             as a shortcut called SM64DS Port. Close Steam
                             first.

    Draw distance (game)     Picture page (Display page in the game). How
                             far away objects are drawn compared with the
                             DS: 1x, 1.5x, 2x, 4x or Max. 1x by default.
                             Offline it also wakes far enemies, which stand
                             still past the DS's own range. Online it only
                             changes what is drawn, so every player sees the
                             same game.

    Debug menu (game)        Only in the game's Settings, General page. Off
                             by default. F5 and Esc open nothing until it is
                             on.


WHAT YOU CAN DO IN IT
---------------------

The game boots to its own title screen and plays from there the way the
cartridge does: pick a file, walk around the castle, go into the courses,
and play the minigames in the Rec Room. Mario, Luigi, Wario and Yoshi are
all there. The Mods page in the launcher installs mods and turns them on and
off. In a VS room each player is a different coloured Yoshi, the timer and
star counts show on screen, and the match ends by naming the winner.

This is a pre-1.0 release. Some things are still rough, and some levels are
further along than others.


CREDITS
-------

Testing: thunderit tested every round of 0.9.3, rounds 1 to 6.

HD power meter art (in the HD UI Textures mod): the original sprite is
Nintendo's, from Super Mario 3D All-Stars. thunderit modified it to match
SM64DS's style.


LINUX AND STEAM DECK (Proton)
-----------------------------

This is a Windows program. It is not built for Linux, and it is not supported
there. One player has reported it running well under Steam's Proton
Experimental (Arch, KDE, Wayland, NVIDIA). That is the only setup anyone has
confirmed. Other Proton builds (GE, dwproton) and plain Wine have not been
confirmed to work, and a Steam Deck has not been tested. If you try it, please
tell us how it went.

What is different from Windows: the launcher unpacks your dump with Windows
PowerShell, and a Proton prefix does not have one. So the unpacking is done
once, outside Proton, with Python 3 (extract_assets.py, included; SteamOS and
most distros already have python3). It makes exactly the same files.

Easiest way, on a desktop with Steam installed:

1. Install "Proton Experimental" in Steam (Library, set the filter to Tools).
2. Copy your .nds file into the folder named PLACE YOUR ROM HERE.
3. Open a terminal in this folder and run:   bash launch.sh
   The first run unpacks the game data (about a minute), then opens the
   launcher inside Proton. Press Play there.

On a Steam Deck (or to start it from Steam):

1. Switch to Desktop Mode. Unzip the kit into your home folder (for example
   into Games), and put your .nds in PLACE YOUR ROM HERE.
2. Double-click "Steam Deck Setup" in the kit folder.
   The first time, KDE asks whether to run it, because files that come out
   of a zip are not marked as programs ("This will start the program: Steam
   Deck Setup"). Press Continue. It only asks once.
   A terminal window opens and the setup:
     - checks your .nds is in PLACE YOUR ROM HERE,
     - unpacks the game data (about a minute, only the first time),
     - asks to close Steam for a moment, adds "Super Mario 64 DS PC" to your
       Steam library set to run with Proton Experimental, and starts Steam
       again. Your old Steam files are kept next to the new ones with .bak-
       and the date at the end of their names.
   Running it again does no harm: it does not add a second entry.
3. Switch to Gaming Mode and find Super Mario 64 DS PC in your library.
   If the setup said Proton Experimental is not installed, install it first:
   Library, set the filter to Tools, Proton Experimental, Install.

If double-clicking does nothing or opens a text editor, open a terminal in the
kit folder (in Dolphin: right-click inside the folder, Open Terminal Here) and
run  python3 steam_deck_setup.py  instead. It is the same thing. A kit on a
memory card formatted as exFAT cannot be marked as a program, so there the
terminal is the way, or unzip the kit to the Deck's own drive.

The same steps by hand, if the setup stops or you prefer to do it yourself:

1. Switch to Desktop Mode, unzip the kit, put your .nds in PLACE YOUR ROM HERE.
2. Open a terminal in the kit folder and run  python3 extract_assets.py  once,
   so the game data is unpacked (about a minute).
3. In Steam choose Add a Non-Steam Game, browse to SM64DSLauncher.exe in the
   kit folder and add it.
4. Open the new entry's Properties, then Compatibility, tick "Force the use of
   a specific Steam Play compatibility tool" and pick Proton Experimental.
5. Run it from Steam, in Desktop Mode or Gaming Mode.

If the picture is black, wrong, or the game closes at start: run  bash
launch.sh --safe  (or set the launch option  SM64DS_RENDERER=0
SM64DS_PRESENT_BACKEND=0 SM64DS_PRESENTER=0 %command%  on the Steam entry). That
draws with the processor and the plain Windows present path, which does not
need Direct3D 11. Without --safe the game is meant to fall back to that by
itself when it cannot get a Direct3D 11 device.

Things to know:
- Updates: the launcher restarts itself after an update with a Windows
  PowerShell helper that Proton does not have. If it just closes, start it
  again (the update is applied on that start).
- Wayland: the one player who got it running did not say whether
  PROTON_ENABLE_WAYLAND mattered. Leave it unset unless X11 gives you trouble.
- Controllers, a Deck's built-in controls, multiplayer and voice chat have
  not been tried under Proton.
- To report a problem, run  bash launch.sh --log  and send the steam-*.log
  file from your home folder, plus what happened. Crash reports sent by the
  launcher are marked as coming from Wine, so we can tell them apart.


IF SOMETHING GOES WRONG
-----------------------

"No .nds file found"
    Your dump is not here yet. Put it in the PLACE YOUR ROM HERE folder. Its
    name has to end in .nds.

"That is not a Super Mario 64 DS dump"
    The .nds file here is a different game, or the dump is damaged.

"This is the US version. The PC port needs the European cartridge for now."
    Your dump is another region's (the message names it). Only the European
    cartridge works for now.

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

    SM64DSLauncher.exe     the launcher: Play, Multiplayer, Mods, settings,
                           updates
    walk_window.exe        the game itself (the launcher starts this)
    PLACE YOUR ROM HERE\   where your .nds dump goes
    README.txt             this file
    launch.sh              Linux and Steam Deck only: starts it under Proton
                           (see above)
    extract_assets.py      Linux and Steam Deck only: unpacks the game data
                           with Python
    Steam Deck Setup       Steam Deck only: double-click in Desktop Mode to
                           set everything up (see above). It runs
    steam_deck_setup.py    which does the work
    extract_assets.ps1     the PowerShell script the launcher uses to unpack
                           your dump on Windows

The rest are small files the launcher and the game read. Leave them as they
are: version.json, launcher_config.json, romdata.manifest,
romdata.recipe.tsv and walk_window.symtab.

Once you have run it, more appear, all made on your PC:

    extracted\           the game's files as your cartridge holds them
    build\assets\        the small index and table files that name them, plus
                         the game's own data tables rebuilt from your dump
    mods\                your installed mods (HD UI Textures lands here after
                         the first start)
    settings.json        your settings
    playlog\             the game's log of each session (play_*.log)

The game itself reads your dump, writes those folders next to itself, and
runs. The launcher checks tangos.dev for updates and for the recommended mods
(HD UI Textures), and if the game crashes it can send a small crash report,
and after each session a short performance report (how the game ran and your
PC's hardware), so problems get fixed; both can be turned off in the
launcher's settings. Multiplayer talks to our server only while you are in a
room, a match or the online world. Nothing else here touches the internet or
changes any setting on your PC.
