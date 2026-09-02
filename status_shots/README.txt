PAL16 screenshots -- for the owner to judge. I have not judged the colours
beyond "distinct", which is a mechanical count, not an opinion about the look.

All five are 512x384 24bpp, the walk_window selftest capture written on the
final frame of a seven-window loopback VS session on arena 0 (level 51), every
window muted and minimized and never activated.

SM64DS_VS_COLORS for both runs (sixteen fields; seats 3, 7, 11 and 15 left
empty so a DEFAULTED seat is exercised in the same session):

  ff0000:0000ff,00ff00:ff00ff,0000ff:ffff00,,ff8800:004488,000000:000000,
  00ffff:880000,,ff00ff:00ff88,8800ff:ffcc00,ffffff:cc0000,,00ff88:660066,
  ff99cc:330066,999999:0044cc,

  seat 0 red      seat 1 green    seat 2 blue     seat 3 (none: ROM row 3)
  seat 4 orange   seat 5 BLACK    seat 6 cyan

still_*   PAL16_STILL=1, nobody holds a direction, so all seven stay in the
          spawn ring and one camera sees the field. 400 frames.
walking_* the default run, each window holding a different direction. 900
          frames per child. The players scatter, so a single camera sees three
          or four of them; kept because it is the run the lockstep sweep and
          the movement assertion were taken from.

WHAT THE PIXEL CENSUS COUNTED in still_p0_parent.bmp, against the exact BGR555
words the running game reported binding for each seat (port/tools/
vs_pal16_census.py):

  seat 0  556 px    seat 1  980 px    seat 2  294 px    seat 3  318 px
  seat 4  484 px    seat 5  see below seat 6  617 px

SEAT 5 COUNTS ZERO AND IS NOT MISSING. Its pick is 000000:000000, so its whole
body ramp is 0000,0000,0000,0000 -- and 0000 is also the background and the
outline in a great many places, so counting it would count the sky. What says
seat 5 is really there is the engine-side probe: it bound base 5525 (row 5) and
submitted 456 triangles per frame over 1300 frames, the same as seat 0. It is a
fully drawn Yoshi that is black.

ONE HONEST NOTE ABOUT THE WINDOWS. still_p1_child.bmp and still_p4_child.bmp
are BYTE-IDENTICAL, and so is every other child's; only the parent's differs.
Every child window renders the same camera in a headless selftest run. That is
a property of the harness and not of this change -- the same pattern is in the
walking run and in this lane's baseline -- and it does not weaken the colour
result, which was read independently inside all seven processes and agreed in
all seven. It does mean "two windows" here is the parent and a child, not seven
genuinely different viewpoints.
