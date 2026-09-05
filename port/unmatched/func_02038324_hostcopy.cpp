// RETIRED, run link100 lane TAIL. This file used to be a HOST COPY of
// src/func_02038324.cpp -- the matched body line for line, plus a bounds guard
// on the collider-slot index it reads out of the surface record. The guard's
// own note said what it was for: the registry has 24 slots and 0x18 is the
// "empty" sentinel, and a record a host probe filled incompletely (the ROM's
// manager wrapper writes result.clsn, direct DetectClsn calls do not) could
// carry an index outside that range, which the ROM's own code would then use
// to index data_020a0c80 and dispatch through whatever it found.
//
// THE MATCHED TU IS IN THE LINK NOW, on slice_gate10.txt where this file's
// line used to be, and the guard is gone with it. That is the point of the
// port -- a defensive branch the ROM does not have is scaffolding, and the
// question it was hedging (can a host-filled record reach here with a bad
// index?) is answered by running the game rather than by keeping the hedge:
// port/tools/battery.py is green with the matched body, which is the same
// evidence any other host copy retirement in this tree rests on.
//
// The file is kept as this note rather than deleted so the retirement is
// visible where the copy was, and so a future run that sees the guard's
// warning line quoted in an old log knows where it went. It defines nothing.
