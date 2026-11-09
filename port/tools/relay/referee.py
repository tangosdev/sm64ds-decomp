#!/usr/bin/env python3
"""THE ARENA REFEREE (run mp1, lane REFEREE1; VSSYNC1 plan section 4, phase A).

In an arena-mode VS session every console runs its own arena world and its own
Yoshi with no input delay; everything two players can disagree about is decided
HERE, inside the relay that already carries their packets: which star wakes
next, who broke a marker, who got a contested star, whether a hit landed, the
score, the clock and the winner. One authority, so there can never be two
winners.

The wire is arena_contract.py's (arena_contract.json / arena_wire.h); this file
carries its own copy of the numbers so the relay box needs nothing else, and
test_referee.py checks the copy against the contract.

What the relay hands the referee (relay.py, an arena session):
  check_state / on_state   every 'SYNS' (48 B): own-body state, 30 Hz
  check_object / on_object every 'SYNO' (32 B): a star in flight
  on_claim                 every 'SYNE' (24 B): a claim, never forwarded
  on_ping                  'SYNP' (20 B, byte 6 = 0xFE): the clock ping
  on_join / on_leave       the relay seated / dropped a member
  on_violation             a seat sent what only the referee may send
  tick                     ~100 Hz: verdicts, the clock, heartbeats
and the referee answers through `send(seat, datagram)` with 'SYNR' (272 B)
event frames and 'SYNQ' (20 B) clock pongs, nothing else.

HOW A CLAIM IS DECIDED. Claims carry the console's referee-clock stamp of the
touch. A stamp is clamped into [arrival - cap, arrival], cap = min(150, the
seat's min RTT / 2 + 25) ms (no claims from the future, no more back-dating
than the seat's own one-way time plus a margin; M3: a cheater who back-dates
every stamp wins 62.5% of ties instead of 50%). Claims are then decided in
STAMP order once the referee clock passes stamp + H, H = the largest cap of the
other seats in the match: by then no other seat can still deliver a claim with
an earlier stamp. Earliest stamp wins; exact ties go to fewer stars, then the
match seed's seat order.

NO AMPLIFIER. The referee sends nothing to a seat until that seat has PROVEN
it receives at its address: its clock ping echoes the exact referee time of a
pong the referee sent to it, and every seat sees the referee clock from its
OWN random base (pongs, claim stamps and every time in a SYNR it receives),
so neither a spoofed source address nor another seat of the same session
can ever echo one. Until
then the seat gets only clock pongs, each the size of the ping that asked.
And whatever a seat has proven, the referee never sends it more than
AMP_FACTOR times the bytes it has sent the referee (a send credit per seat,
the QUIC rule, capped at AMP_CREDIT_CAP): a seat that goes quiet stops
hearing the referee, so even a seat verified by a trick (REVREFEREE1 H1: one
real seat reading the session clock for forged ones) cannot be turned into a
reflector. A live console streams SYNS at 10-30 Hz, three times which is
1.4-4.3 KB/s of credit against a 1.1 KB/s heartbeat.

SECURITY. Fixed-size kinds only (the relay checks tag + exact length before
anything here runs); every field is range-checked; every table is bounded
(history 64 samples a seat, 64 decided claims a seat, 64 retained events, 255
star instances, a pending queue capped per seat); nothing a client says that
the referee can check is trusted (the ledger is the referee's, a SYNS carries
no score, a grab / hit is checked against the claimant's and victim's OWN
reported positions). Phase A logs speed strikes; kicking is phase B.

Python 3 standard library only.
"""

import math
import os
import random
import struct
from collections import deque

# ---------------------------------------------------------------- numbers
# (arena_contract.py NUMBERS; test_referee.py checks this copy)

FRAME_MS = 1000.0 / 30.0
HEARTBEAT_HZ = 4
CLAIM_RESEND_MS = 60
VERDICT_TIMEOUT_MS = 500
HOLD_MAX_MS = 150
CAP_MARGIN_MS = 25
CAP_UNKNOWN_MS = 100
FUTURE_SLACK_MS = 5
COMP_MAX_MS = 200
HIT_IMMUNE_MS = 1000
STAR_POSE_MS = 1500             # FIX9C: the star-get pose (the ROM's no-control hold) is hit-immune this long after the grab
# an ESCAPE earlier than this after the EATEN is refused: the ROM's victim
# needs 23 or more frames of mashing after its own console has the event, so a
# real escape is never this fast (a cheating victim could otherwise make its
# eater lose a star one round trip after being eaten)
ESCAPE_MIN_MS = int(15 * FRAME_MS)
TRADE_MS = 34
# A star in flight is judged looser than one at rest: the referee's copy of it
# is the owner's newest SYNO (one trip old) and the claimant's own copy is a
# second trip behind that, so while it moves the two can be several frames of
# flight apart (FIELD4 F9-b). The slack is added to GRAB_R, never replaces it:
# a claim from far outside both still fails (the cheat control).
FLIGHT_SLACK = 300
FLIGHT_STALE_MS = 1000          # a "moving" SYNO older than this proves nothing
DENY_LOG_PER_S = 4              # journal lines for refused claims, per session
REACH_H = 250
REACH_V = 250
GRAB_R = 200
MARKER_R = 300
ATTACK_LOOKBACK_FRAMES = 10
PRESENCE_MS = 3000
START_WAIT_MS = 20000
COUNTDOWN_MS = 3000
OVER_LINGER_MS = 3000
SNAPSHOT_LAG_MS = 2000
HISTORY_FRAMES = 64
INVULN_CHEAT_MS = 300
STRIKE_WINDOW_MS = 10000
STRIKES_TO_KICK = 3
POS_LIMIT_UNITS = 32768
GRAB_STUN_MAX_MS = 5000         # a catch needs the Wario hit's stun this fresh
GRAB_RELEASE_MIN_MS = 500       # a hold lasts at least this long
GRAB_STUCK_MS = 3000            # a hold nobody reports for this long is let go
GRAB_SPEED_MAX = 0x40000        # the fastest launch the ROM's throws can give
STOMP_MIN_MS = 250              # one stomp per stomper and victim per this long
STOMP_REACH_H = 130             # body radii 40 + 40, a margin for the drawn frame
STOMP_ABOVE_MIN = 10            # the ROM's 0xa000 (10 units): the stomper this far above
CAP_R = 200                     # a cap is taken from this far (cap cylinder 50 + body 40, margin)
CAP_FLIGHT_SLACK = 1500         # a knocked-off cap flies and slides (down a slope too): it is found this far from the pop point
CAP_FLIGHT_MS = 7000            # ... for as long as it lies (CAP_LIFE_MS)
CAP_RESPAWN_MS = 1500           # a taken placed cap is back this long after the stamp
CAP_LIFE_MS = 7000              # a knocked-off cap lies this long (the ROM's 210 frames)
CAP_WORN_MS = 3000              # CAP_DROP: the seat's own SYNS showed that cap this recently
CAP_HURT_EVIDENCE_MS = 1500     # ... and showed a hurt this recently
CAP_DROP_WAIT_MS = 150          # a CAP_DROP whose evidence has not arrived yet waits this long
CAP_DROP_MIN_MS = 1000          # one CAP_DROP per seat per this long
# LUIGI INFECTION (mode 2, lane REFLUIGI1)
LUIGI_COUNTDOWN_MS = 5000       # mode 2's start countdown: the first Luigi is picked at T0, five seconds on (Classic's)
TAG_REACH_H = 130               # a TAG is body contact: radii 40 + 40, a margin for the drawn frame
TAG_REACH_V = 200               # ... and a body's height with a margin

FIX = 4096                      # Fix12
POS_LIMIT = POS_LIMIT_UNITS * FIX
SEATS = 16
REF_MARK = 0xFE
SAMPLE_TOLERANCE_FRAMES = 15    # a claim's frame must have a sample this near
RETAIN_EVENTS = 64
DONE_PER_SEAT = 64
PENDING_PER_SEAT = 16
CATCHUP_EVERY_MS = 250
RTT_KEEP = 8
AMP_FACTOR = 3                  # referee bytes out <= 3 x the seat's bytes in
AMP_CREDIT_CAP = 16384          # the most credit a seat can bank
RESET_BACK_FRAMES = 300         # a frame this far back = the console restarted

# ---------------------------------------------------------------- wire

TAG_SYNS, TAG_SYNO, TAG_SYNE = b"SYNS", b"SYNO", b"SYNE"
TAG_SYNR, TAG_SYNP, TAG_SYNQ = b"SYNR", b"SYNP", b"SYNQ"
SYNS_LEN, SYNO_LEN, SYNE_LEN, PING_LEN = 48, 32, 24, 20
SYNR_EVENTS = 8
SYNR_LEN = 48 + 28 * SYNR_EVENTS

S_SYNS = struct.Struct("<4sBBBBI3i3hhHHBBBBBBH2BH")
S_SYNO = struct.Struct("<4sBBBBI3i3hBB")
S_SYNE = struct.Struct("<4sBBBBHHIII")
S_PING = struct.Struct("<4sBBBBIIHH")
S_HDR = struct.Struct("<4sBBBBIHHII16bBBBBBBBB")
S_EVT = struct.Struct("<HBBBBHHHIiii")
_U32 = struct.Struct("<I")
assert (S_SYNS.size, S_SYNO.size, S_SYNE.size, S_PING.size, S_HDR.size,
        S_EVT.size) == (48, 32, 24, 20, 48, 28)

# SYNS flags (plan 2.1 bit order) and power bits
F_LIVE, F_GROUNDED, F_TELEPORT, F_PAUSED = 0x01, 0x02, 0x04, 0x08
F_TALKING, F_IN_HURT, F_NO_CONTROL, F_STAR_LOCK = 0x10, 0x20, 0x40, 0x80
P_FIRE, P_WINGS, P_METAL, P_VANISH, P_MEGA = 0x01, 0x02, 0x04, 0x08, 0x10

# claims
C_READY, C_MARKER_BREAK, C_GRAB, C_HIT, C_HURT_SELF, C_PIT, C_QUIT = range(1, 8)
C_EAT, C_SPIT, C_SWALLOW, C_ESCAPE = 8, 9, 10, 11
C_CATCH, C_THROW, C_SWING, C_BREAK, C_STOMP = 12, 13, 14, 15, 16
C_CAP_TAKE, C_CAP_DROP = 17, 18
C_TAG = 19
CLAIM_NAMES = {1: "READY", 2: "MARKER_BREAK", 3: "GRAB", 4: "HIT",
               5: "HURT_SELF", 6: "PIT", 7: "QUIT", 8: "EAT", 9: "SPIT",
               10: "SWALLOW", 11: "ESCAPE", 12: "CATCH", 13: "THROW",
               14: "SWING", 15: "BREAK", 16: "STOMP", 17: "CAP_TAKE",
               18: "CAP_DROP", 19: "TAG"}
# events
(EV_START, EV_STAR_WAKE, EV_MARKER_BROKEN, EV_STAR_TAKEN, EV_DENY, EV_HIT,
 EV_CLASH, EV_SPILL, EV_MATCH_OVER, EV_LEAVE, EV_JOIN, EV_STAR_STATE,
 EV_ACCEPTED, EV_EATEN, EV_RELEASED, EV_GRABBED, EV_THROWN,
 EV_STOMPED, EV_CAP_TAKEN, EV_CAP_DROPPED, EV_CAP_STATE,
 EV_INFECTED, EV_TEAM_STATE) = range(1, 24)
# deny reasons
(D_STALE, D_GONE, D_LOST, D_RANGE, D_STATE, D_INVULN, D_MISMATCH,
 D_UNSUPPORTED, D_BAD, D_TOO_OLD, D_SELF, D_TEAM) = range(1, 13)
DENY_NAMES = {1: "STALE", 2: "GONE", 3: "LOST", 4: "RANGE", 5: "STATE",
              6: "INVULN", 7: "MISMATCH", 8: "UNSUPPORTED", 9: "BAD",
              10: "TOO_OLD", 11: "SELF", 12: "TEAM"}
# match-over reasons
O_TIME_UP, O_FIRST_TO_N, O_LAST_STANDING, O_EVERYONE_LEFT = 1, 2, 3, 4
O_LUIGIS_WIN, O_SURVIVORS_WIN = 5, 6
# READY modes (rules byte 1): 1 is kept for King of the Star and refused
MODE_CLASSIC, MODE_KING, MODE_LUIGI = 0, 1, 2
MODES_ACCEPTED = (MODE_CLASSIC, MODE_LUIGI)
# INFECTED field b
INF_PICK, INF_HIT, INF_STOMP, INF_TAG, INF_HANDOFF, INF_LATE_JOIN = range(1, 7)
LOOK_LUIGI = 1
# match states
MS_WAITING, MS_COUNTDOWN, MS_RUNNING, MS_OVER = 0, 1, 2, 3
# SYNR header flags
RF_SNAPSHOT, RF_DIRECTED, RF_RESEND = 0x01, 0x02, 0x04
# star instance states
ST_WAKING, ST_FREE = 1, 2
# spill causes
SP_HIT, SP_LEAVE, SP_HAZARD, SP_PIT = 1, 2, 3, 4
# leave reasons
L_QUIT, L_SILENT, L_DETACH = 1, 2, 3

STATE_NAMES = [
    "St_LedgeHang", "St_LedgeGrab", "St_CeilingGrate", "St_YoshiPower",
    "St_Swallow", "St_InYoshiMouth", "St_SmallLaunchUp", "St_Hurt",
    "St_HurtWater", "St_Electrocute", "St_BurnFire", "St_BurnLava",
    "St_DeadHit", "St_DeadPit", "St_Walk", "St_Wait", "St_Grabbed",
    "St_TurnAround", "St_Jump", "St_Fall", "St_Thrown", "St_SideFlip",
    "St_SlideKickRecover", "St_Fly", "St_NoControl", "St_Owl", "St_DebugFly",
    "St_WindCarry", "St_Balloon", "St_Teleport", "St_TornadoSpin", "St_Cannon",
    "St_Squish", "St_Shell", "St_StomachSlide", "St_ButtSlide",
    "St_DizzyStars", "St_HoldLight", "St_Bonk", "St_HoldHeavy", "St_WallSlide",
    "St_BowserEarthquake", "St_WallJump", "St_SlopeJump", "St_StuckInGround",
    "St_Land", "St_OnWall", "St_Spin", "St_Talk", "St_CrazedCrate",
    "St_WaitQuicksand", "St_LevelEnter", "St_JumpQuicksand", "St_Crouch",
    "St_Respawn", "St_Crawl", "St_BackFlip", "St_OpeningWakeUp", "St_LongJump",
    "St_PunchKick", "St_EndingFly", "St_GroundPound", "St_Dive", "St_Throw",
    "St_GrabBowserTail", "St_SwingPlayer", "St_SweepKick", "St_SlideKick",
    "St_CameraZoom", "St_Null", "St_Swim", "St_WaterJump",
    "St_MetalWaterGround", "St_MetalWaterWater", "St_Climb", "St_Headstand",
    "St_WallJump", "St_HeadstandJump",
]
N_STATES = len(STATE_NAMES)
ATTACK_STATES = ["St_YoshiPower", "St_Spin", "St_PunchKick", "St_GroundPound",
                 "St_Dive", "St_SweepKick", "St_SlideKick",
                 "St_SlideKickRecover", "St_Land", "St_SwingPlayer"]
DISABLED_STATES = ["St_Hurt", "St_HurtWater", "St_Electrocute", "St_BurnFire",
                   "St_BurnLava", "St_DeadHit", "St_DeadPit",
                   "St_InYoshiMouth", "St_NoControl", "St_Squish",
                   "St_DizzyStars", "St_Respawn", "St_Thrown", "St_Grabbed"]
CLASH_STATES = ["St_PunchKick"]
# The one attack state in which the ROM itself raises mIsNoControl: a tongue
# that has caught something (the St_Swallow / YoshiPower helpers call
# Player_DisableInteraction; no other state in ATTACK_STATES has a caller, see
# the relocation table of ov002 for 0x020c9e40). An attacker standing in it
# with that flag up is honest, so _usable() lets it claim (HITSPILL1).
NC_ATTACK_STATES = ["St_YoshiPower"]
ATTACK_NC_MAX_FRAMES = 45       # the exemption lasts at most 1.5 s of one run
SPEED_WALK = ["St_Walk", "St_Wait", "St_TurnAround", "St_Crouch", "St_Crawl",
              "St_PunchKick", "St_YoshiPower", "St_HoldLight", "St_HoldHeavy",
              "St_Throw", "St_Talk", "St_Spin"]
SPEED_SLIDE = ["St_StomachSlide", "St_ButtSlide", "St_SlideKick",
               "St_SlideKickRecover", "St_Dive"]
SPEED_EXEMPT = ["St_InYoshiMouth", "St_Respawn", "St_Thrown", "St_Teleport",
                "St_Cannon", "St_DeadPit", "St_SmallLaunchUp", "St_Swallow",
                "St_LevelEnter"]
SPEED_LIMITS = {"walk": 50, "slide": 125, "air": 90}
SPEED_AVG_LIMIT = 60
SPEED_AVG_FRAMES = 10

_IDX = dict((n, i) for i, n in reversed(list(enumerate(STATE_NAMES))))
_ALL_IDX = {}
for _i, _n in enumerate(STATE_NAMES):
    _ALL_IDX.setdefault(_n, set()).add(_i)


def _idx_set(names):
    out = set()
    for n in names:
        out |= _ALL_IDX[n]
    return frozenset(out)


ATTACK_IDX = _idx_set(ATTACK_STATES)
NOCONTROL_IDX = _idx_set(["St_NoControl"])
DISABLED_IDX = _idx_set(DISABLED_STATES)
CLASH_IDX = _idx_set(CLASH_STATES)
SPEED_CLASS = []
for _i, _n in enumerate(STATE_NAMES):
    if _n in SPEED_EXEMPT:
        SPEED_CLASS.append(None)
    elif _n in SPEED_SLIDE:
        SPEED_CLASS.append("slide")
    elif _n in SPEED_WALK:
        SPEED_CLASS.append("walk")
    else:
        SPEED_CLASS.append("air")
HURT_IDX = _idx_set(["St_Hurt", "St_HurtWater"])
# VSREF3: any state a hurt body is in (the ROM pops the cap from St_Hurt_Init)
CAP_HURT_IDX = _idx_set(["St_Hurt", "St_HurtWater", "St_Electrocute",
                         "St_BurnFire", "St_BurnLava", "St_DeadHit"])
# GRAB1: a catch needs a Wario in a punch or a dive, and a victim in the stun a
# Wario hit leaves (St_Hurt, phase 2) or in St_DizzyStars; a hold shows as
# St_HoldLight / St_SwingPlayer / St_Throw on the holder, St_Grabbed on the victim
CATCH_ATTACK_IDX = _idx_set(["St_PunchKick", "St_Dive"])
STUN_IDX = _idx_set(["St_Hurt", "St_DizzyStars"])
HURT_STUN_IDX = _idx_set(["St_Hurt"])
HOLDER_IDX = _idx_set(["St_HoldLight", "St_SwingPlayer", "St_Throw",
                       "St_PunchKick", "St_Dive", "St_Swim"])
GRABBED_IDX = _idx_set(["St_Grabbed", "St_Thrown"])
LOOK_WARIO = 2
GROUND_POUND_IDX = _idx_set(["St_GroundPound"])
YOSHI_POWER_IDX = _idx_set(["St_YoshiPower"])
NC_ATTACK_IDX = _idx_set(NC_ATTACK_STATES)

# The caps each arena map places, cut from the ROM's own level data (VSREF3
# census): map -> [(slot, x, y, z, param1)] in whole units; slot is the actor's
# death-table id (Actor+0xce, distinct for every cap of a map, so five caps
# stacked on one spot stay apart) and the cap's id is slot + 1. param1 is the
# ROM's own (type | model << 8). A map with no entry is judged without a table:
# the first claim names the cap (its place hash) and nothing can be range-checked.
CAP_MAPS = {
    0: [
        (2, -1299, 729, 3236, 0x204),
        (3, 6048, 449, 414, 0x004),
        (4, 2292, 43, 4544, 0x104),
        (5, -4562, 590, 4628, 0x204),
        (6, 150, 964, 725, 0x004),
        (7, 804, 647, 2974, 0x104),
    ],
    1: [
        (5, -100, 853, -1900, 0x204),
        (6, -900, 590, -1300, 0x104),
        (7, 341, 356, -1106, 0x004),
        (8, -2614, 158, 2208, 0x004),
        (9, 297, 100, 3606, 0x104),
        (10, 3700, 94, 300, 0x204),
    ],
    2: [
        (0, 3750, 6008, -5800, 0x204),
        (1, 3750, 6008, -5200, 0x104),
        (2, 3750, 6000, -5500, 0x004),
        (3, -6214, -4380, 5849, 0x204),
        (4, -5938, -4380, 5361, 0x104),
        (5, -6476, -4380, 5341, 0x004),
        (6, -6214, -4380, 5849, 0x204),
        (7, -5938, -4380, 5361, 0x104),
        (8, -6476, -4380, 5341, 0x004),
        (9, -6214, -4380, 5849, 0x204),
        (10, -5938, -4380, 5361, 0x104),
        (11, -6476, -4380, 5341, 0x004),
        (12, -6214, -4380, 5849, 0x204),
        (13, -5938, -4380, 5361, 0x104),
        (14, -6476, -4380, 5341, 0x004),
    ],
    3: [
        (5, -300, 0, -300, 0x004),
        (6, -1050, 408, 1000, 0x104),
        (7, 1000, 408, -1050, 0x204),
        (8, -300, 0, -300, 0x004),
        (9, -400, 0, 1800, 0x104),
        (10, 1800, 0, -400, 0x204),
    ],
}
CAP_PLACED_MAX = 31             # ids 1..31 are placed caps, 32..255 knocked-off ones
CAP_DROP_BASE = 32
CAP_POP_UP = 0x82000            # the ROM's pop point: the body's y + 130 units
LOOK_NONE = 0xFF

STAR_ORDERS = [
    [0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 3, 2, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 2, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 2, 3, 1, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 3, 2, 1, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 3, 1, 2, 4, 0, 0, 0, 0, 0, 0, 0],
]


def env_flag(name):
    return os.environ.get(name, "") == "1"


def seq_next(seq):
    seq = (seq + 1) & 0xFFFF
    return 1 if seq == 0 else seq


def seq_after(a, b):
    """How many stream steps b is past a (0 when equal), in 1..65535."""
    return (b - a) % 0xFFFF if a else (b if b else 0)


# ---------------------------------------------------------------- tables


# Every refusal the referee has named since the process started, by claim kind
# and reason: relay.py prints it on the stats line ("arena_deny=GRAB.RANGE:3,...")
# so a field test can say which rule refused what without a guess.
DENY_TOTALS = {}


def deny_totals_text():
    return ",".join("%s.%s:%d" % (CLAIM_NAMES.get(k, str(k)),
                                  DENY_NAMES.get(r, str(r)), n)
                    for (k, r), n in sorted(DENY_TOTALS.items()))


class Sample(object):
    __slots__ = ("frame", "rt", "x", "y", "z", "yaw", "state", "step",
                 "invuln", "power", "flags", "held", "eaten_by", "look", "vy")


class Seat(object):
    """What the referee knows about one relay seat."""

    def __init__(self, seat, rt):
        self.seat = seat
        self.joined = rt
        self.verified = False       # echoed one of our pongs (no amplifier)
        self.base = 0               # this seat's own clock base (on_join)
        self.credit = 0             # bytes the referee may still send it
        self.pongs = deque(maxlen=RTT_KEEP)   # t_ref values we sent it
        self.hist = deque(maxlen=HISTORY_FRAMES)
        self.last_heard = rt
        self.ready = None           # the READY config tuple, once sent
        self.ack = 0
        self.ack_lag_since = None
        self.last_catchup = -1e9
        self.last_snapshot = -1e9
        self.rtts = deque(maxlen=RTT_KEEP)
        self.rtt_min = None
        self.done = {}              # claim_id -> (seq, record) of its verdict
        self.done_order = deque()
        self.pending_ids = set()
        self.clamps = 0
        self.claims = 0
        self.strikes = deque()
        self.strikes_total = 0
        self.last_hit = None        # referee stamp of the last HIT on me
        self.star_stamp = None      # FIX9C: referee stamp of my last granted star (the pose after it is hit-immune)
        self.last_hit_by = None
        self.hit_check = None       # (deadline rt, verdict rt)
        self.sanction_until = -1e9
        self.eaten_by = None        # the referee's view (provisional, A5)
        self.eaten_at = -1e9        # referee ms of the EATEN that set it
        self.grabbed_by = None      # GRAB1: the seat holding me (Wario hold)
        self.grabbed_at = -1e9      # GRAB1: referee ms of the GRABBED that set it
        self.holding = None         # GRAB1: the seat I hold
        self.last_stomp = {}        # GRAB1: victim seat -> referee ms of my last STOMP on it
        self.eat_stamp = None       # stamp of the last EAT this seat won ...
        self.eat_target = None      # ... and whom it ate (HIT vs EAT order)
        self.speed_win = deque(maxlen=SPEED_AVG_FRAMES)
        self.speed_logged = -1e9
        self.worn_look = None       # VSREF3: the cap look my own SYNS showed last (0..2)
        self.worn_rt = -1e9         # ... and when
        self.drop_frame = -1        # my_frame of my last granted CAP_DROP
        self.drop_rt = -1e9         # ... and when

    def cap(self):
        if self.rtt_min is None:
            return CAP_UNKNOWN_MS
        return min(HOLD_MAX_MS, self.rtt_min // 2 + CAP_MARGIN_MS)

    def one_way(self):
        return 50 if self.rtt_min is None else self.rtt_min / 2.0

    def newest(self):
        return self.hist[-1] if self.hist else None

    def sample_at(self, frame, tol=SAMPLE_TOLERANCE_FRAMES):
        """The newest sample at or before `frame`, else the oldest after it,
        within `tol` frames; None if there is none that close."""
        best = None
        for s in reversed(self.hist):
            if s.frame <= frame:
                best = s
                break
        if best is None:
            for s in self.hist:
                if s.frame > frame:
                    best = s
                    break
        if best is None or abs(best.frame - frame) > tol:
            return None
        return best

    def samples_between(self, lo, hi):
        return [s for s in self.hist if lo <= s.frame <= hi]


class Star(object):
    __slots__ = ("inst", "state", "marker", "star_id", "x", "y", "z", "known",
                 "owner", "since", "moving", "syno_rt")

    def __init__(self, inst, state, marker, star_id, rt):
        self.inst = inst
        self.state = state
        self.marker = marker
        self.star_id = star_id
        self.x = self.y = self.z = 0
        self.known = False
        self.owner = 0xFF
        self.since = rt
        self.moving = False     # the owner's newest SYNO said "in flight"
        self.syno_rt = -1e9     # referee ms that SYNO arrived


class Claim(object):
    __slots__ = ("seat", "kind", "target", "cid", "aux", "stamp", "frame",
                 "ref_frame", "arrival", "resolve_at", "order")


class Cap(object):
    """One cap the referee tracks: a cap the arena places (ids 1..31) or one a
    hit knocked off a player's head (32..255)."""
    __slots__ = ("cid", "placed", "x", "y", "z", "known", "look", "hash",
                 "taken", "taken_by", "taken_stamp", "avail_at", "born",
                 "expires")

    def __init__(self, cid, placed, rt):
        self.cid = cid
        self.placed = placed
        self.x = self.y = self.z = 0
        self.known = False          # its position is on record (range checks)
        self.look = LOOK_NONE
        self.hash = 0
        self.taken = False
        self.taken_by = 0xFF
        self.taken_stamp = 0
        self.avail_at = 0
        self.born = rt
        self.expires = 0


class Referee(object):
    """One arena session's referee. `send(seat, data)` delivers a datagram to
    a seat through the relay; `log(msg)` writes one line."""

    def __init__(self, code, now, send, log, seed=None, kick=None):
        self.code = code
        self.created = now
        # the wire clock's random base (see NO AMPLIFIER); below 2^31 so a
        # session never wraps the u32 inside 24 days
        if seed is not None:
            self.base = (seed * 2654435761) & 0x7FFFFFFF
        else:
            self.base = int.from_bytes(os.urandom(4), "little") & 0x7FFFFFFF
        self.send = send
        self.log = log
        self.kick = kick            # callable(seat) or None: phase A logs only
        self.kick_on = kick is not None     # the relay passes one only when
                                            # SM64DS_REFEREE_KICK=1
        self.seed_env = seed
        self._base_rng = random.Random((seed or 0) ^ 0xBA5E5EED)
        self.seats = {}             # seat -> Seat (every seated member)
        self.order_counter = 0
        self.reset_match(self.rt(now))
        # counters
        self.n_states = 0
        self.n_bad_states = 0
        self.n_claims = 0
        self.n_verdicts = 0
        self.n_pings = 0
        self.n_tx = 0
        self.n_starved = 0
        self.n_violations = 0
        self.last_beat = -1e9
        self.deny_counts = {}       # (claim kind, reason) -> n, this session
        self._dlog_t = -1e9         # journal budget for refusal lines
        self._dlog_n = 0

    # ---- clocks

    def rt(self, now):
        """Referee ms since this session was created (float)."""
        return (now - self.created) * 1000.0

    def ref_ms(self, now, seat=None):
        """The referee clock now: the session's wire base, or the u32 one
        seat sees (its own base) when `seat` is given."""
        if seat is None:
            return self.wire(self.rt(now))
        return self.seat_wire(seat, self.rt(now))

    def seat_wire(self, seat, rt):
        s = self.seats.get(seat)
        return ((s.base if s else self.base) + int(rt)) & 0xFFFFFFFF

    def seat_unwire(self, seat, v):
        s = self.seats.get(seat)
        d = (v - (s.base if s else self.base)) & 0xFFFFFFFF
        return d - 0x100000000 if d >= 0x80000000 else d

    def wire(self, rt):
        """Internal referee ms -> the u32 the wire carries."""
        return (self.base + int(rt)) & 0xFFFFFFFF

    def unwire(self, v):
        """A wire time -> internal referee ms (negative = before the
        session began)."""
        d = (v - self.base) & 0xFFFFFFFF
        return d - 0x100000000 if d >= 0x80000000 else d

    # ---- match lifecycle

    def reset_match(self, rt):
        self.state = MS_WAITING
        self.cfg = None
        self.first_ready = None
        self.seed = 0
        self.order_row = 0
        self.rng = random.Random(0)
        self.seat_rank = list(range(SEATS))
        self.t0 = 0
        self.match_ms = 0
        self.roster = set()
        self.had = 0
        self.scores = [0] * SEATS
        self.progress = 0
        self.stars = {}
        self.caps = {}              # VSREF3: cap id -> Cap
        self.cap_wait = []          # CAP_DROP claims waiting for their evidence
        self.next_cap = CAP_DROP_BASE
        self.taken = {}             # inst -> (winner seat, stamp), recent
        self.taken_order = deque()
        self.broken = {}            # inst -> breaker seat
        self.next_inst = 1
        self.pending = []
        # A rematch in the same session starts a clean window but keeps the
        # sequence running, so a console from the last match applies the new
        # START in order instead of waiting for a SNAPSHOT (REVREFEREE1).
        self.events = deque(maxlen=RETAIN_EVENTS)   # (seq, record)
        self.newest_seq = getattr(self, "newest_seq", 0)
        self.unsent = 0
        self.over_at = None
        self.over_event = None
        self.luigis = set()         # LUIGI: the team, the referee's alone
        self.picked = False         # LUIGI: the first Luigi has been picked
        for s in self.seats.values():
            s.ready = None
            s.done.clear()
            s.done_order.clear()
            s.pending_ids.clear()
            s.last_hit = None
            s.star_stamp = None
            s.eaten_by = None
            s.grabbed_by = None
            s.holding = None
            s.last_stomp = {}
            s.eat_stamp = s.eat_target = None
            s.worn_look, s.worn_rt, s.drop_frame, s.drop_rt = (
                None, -1e9, -1, -1e9)

    def _start(self, rt):
        cfg = self.cfg
        if self.seed_env is not None:
            seed = self.seed_env & 0xFFFFFFFF
        else:
            seed = int.from_bytes(os.urandom(4), "little")
        self.seed = seed
        self.order_row = seed % 6
        self.rng = random.Random(seed)
        rank = list(range(SEATS))
        random.Random(seed ^ 0x5EED).shuffle(rank)
        self.seat_rank = rank       # tie order: lower rank wins
        self.roster = set(s.seat for s in self.seats.values()
                          if s.ready == cfg)
        self.had = len(self.roster)
        self.t0 = int(rt) + (LUIGI_COUNTDOWN_MS if self._luigi()
                             else COUNTDOWN_MS)
        self.match_ms = cfg[1] * 1000
        self.state = MS_COUNTDOWN
        for s in self.roster:
            self.seats[s].last_heard = max(self.seats[s].last_heard,
                                           float(self.t0))
        self._emit(EV_START, 0xFF, self.order_row, cfg[2], cfg[1],
                   self._mask(self.roster), cfg[5], self.t0, seed, cfg[0],
                   self._rules_word(cfg))
        self.log("arena %s START seats=%s map=%d time=%ds first_to=%d row=%d"
                 % (self.code, sorted(self.roster), cfg[0], cfg[1], cfg[2],
                    self.order_row))
        self._wake_next(rt)
        self._cap_table(cfg[0], rt)

    def _cap_table(self, map_id, rt):
        """The arena's placed caps, ids 1.. in table order."""
        self.caps = {}
        for (slot, x, y, z, param1) in CAP_MAPS.get(map_id, ()):
            if not 0 <= slot < CAP_PLACED_MAX:
                continue
            cap = Cap(slot + 1, True, rt)
            cap.x, cap.y, cap.z = x * FIX, y * FIX, z * FIX
            cap.known = True
            look = (param1 >> 8) & 0xF
            cap.look = look if look < 3 else LOOK_NONE
            cap.hash = self.cap_hash(slot, x, z)
            self.caps[cap.cid] = cap

    @staticmethod
    def cap_hash(slot, x_units, z_units):
        """The place hash a CAP_TAKE carries for a placed cap."""
        return (int(slot) & 0xFF) | (((int(x_units) ^ int(z_units)) & 0xFF) << 8)

    @staticmethod
    def _rules_word(cfg):
        return cfg[2] | (cfg[3] << 8) | (cfg[4] << 16) | (cfg[5] << 24)

    @staticmethod
    def _mask(seats):
        m = 0
        for s in seats:
            m |= 1 << s
        return m

    def _over(self, rt, reason, t=None):
        if self.state == MS_OVER:
            return
        # anything still waiting is decided now or never
        for c in list(self.pending):
            self._deny(c, D_STALE, rt)
        self.pending = []
        self.state = MS_OVER
        self.over_at = rt
        if reason == O_LUIGIS_WIN:
            winners = sorted(self.roster & self.luigis)
        elif reason == O_SURVIVORS_WIN:
            winners = sorted(self.roster - self.luigis)
        else:
            top = max([self.scores[s] for s in self.roster] or [0])
            winners = [s for s in sorted(self.roster)
                       if self.scores[s] == top]
        mask = self._mask(winners)
        end = int(rt) if t is None else int(t)
        self.over_event = self._emit(EV_MATCH_OVER, 0xFF, reason,
                                     len(winners), mask, 0, 0,
                                     end, 0, 0, 0)
        self.log("arena %s MATCH_OVER reason=%d winners=%s scores=%s"
                 % (self.code, reason, winners,
                    [self.scores[s] for s in sorted(self.roster)]))

    # ---- events

    def _emit(self, etype, seat, a=0, b=0, c=0, d=0, e=0, t=0, x=0, y=0, z=0):
        self.newest_seq = seq_next(self.newest_seq)
        rec = S_EVT.pack(self.newest_seq, etype, seat & 0xFF, a & 0xFF,
                         b & 0xFF, c & 0xFFFF, d & 0xFFFF, e & 0xFFFF,
                         self.wire(t), _s32(x), _s32(y), _s32(z))
        self.events.append((self.newest_seq, rec))
        self.unsent += 1
        return (self.newest_seq, rec)

    def _snap_record(self, etype, seat, a=0, b=0, c=0, d=0, e=0, t=0, x=0,
                     y=0, z=0):
        return S_EVT.pack(0, etype, seat & 0xFF, a & 0xFF, b & 0xFF,
                          c & 0xFFFF, d & 0xFFFF, e & 0xFFFF, self.wire(t),
                          _s32(x), _s32(y), _s32(z))

    def _frame(self, rt, records, flags=0, part=0, seat=None):
        """One SYNR for `seat`: the header and up to 8 event records, newest
        first, every time rendered in that seat's own clock base (records
        are kept in the session base and re-based here)."""
        recs = list(records)[:SYNR_EVENTS]
        if seat is not None:
            out = []
            for rec in recs:
                t = self.unwire(_U32.unpack_from(rec, 12)[0])
                out.append(rec[:12] + _U32.pack(self.seat_wire(seat, t))
                           + rec[16:])
            recs = out
            wire = lambda v: self.seat_wire(seat, v)
        else:
            wire = self.wire
        hdr = S_HDR.pack(TAG_SYNR, 1, REF_MARK, len(recs), self.state,
                         wire(rt), self.newest_seq,
                         self._mask(self.roster),
                         wire(self.t0) if self.t0 else 0,
                         self.match_ms & 0xFFFFFFFF,
                         *[max(-128, min(127, v)) for v in self.scores],
                         self.progress & 0xFF,
                         (self.cfg[2] if self.cfg else 0) & 0xFF,
                         flags, part, self.order_row,
                         (self.cfg[0] if self.cfg else 0) & 0xFF,
                         min(255, len(self.stars)), 0)
        body = b"".join(recs) + b"\x00" * (28 * (SYNR_EVENTS - len(recs)))
        return hdr + body

    def _tx(self, seat, data):
        s = self.seats.get(seat)
        if s is None or not s.verified:
            return      # not proven to receive at its address: nothing
        if s.credit < len(data):
            self.n_starved += 1     # quiet seat: it hears the referee again
            return                  # once it sends (heartbeats re-carry all)
        s.credit -= len(data)
        self.n_tx += 1
        self.send(seat, data)

    @staticmethod
    def _earn(s, n):
        s.credit = min(AMP_CREDIT_CAP, s.credit + AMP_FACTOR * n)

    def _broadcast(self, rt, records, flags=0):
        for s in sorted(self.seats):
            if self.seats[s].verified:
                self._tx(s, self._frame(rt, records, flags, seat=s))

    def _window(self):
        return [rec for seq, rec in reversed(self.events)][:SYNR_EVENTS]

    def _flush(self, rt):
        """Send what was made since the last send: every new event rides in at
        least one datagram, newest first, 8 a datagram."""
        n = self.unsent
        self.unsent = 0
        if n <= 0:
            return
        evs = list(self.events)
        n = min(n, len(evs))
        new = evs[len(evs) - n:]
        # oldest chunk first so a console sees them in order
        while new:
            chunk = new[:SYNR_EVENTS]
            new = new[SYNR_EVENTS:]
            if not new:
                # the last datagram carries the 8 newest (redundancy)
                self._broadcast(rt, self._window())
            else:
                self._broadcast(rt, [rec for seq, rec in reversed(chunk)])
        self.last_beat = rt

    # ---- the relay's hooks

    def body_count(self):
        """Bodies the match is set up for (the first READY's capacity), or None
        while no READY has fixed it."""
        return self.cfg[4] if self.cfg is not None else None

    def on_join(self, seat, now):
        # Nothing is sent yet: the seat hears the referee once its clock ping
        # proves its address (on_ping).
        s = Seat(seat, self.rt(now))
        if self.seed_env is not None:
            s.base = self._base_rng.getrandbits(31)
        else:
            s.base = int.from_bytes(os.urandom(4), "little") & 0x7FFFFFFF
        self.seats[seat] = s

    def on_leave(self, seat, now, why=L_DETACH):
        rt = self.rt(now)
        if seat in self.roster:
            self._drop_from_match(seat, rt, why)
            self._flush(rt)
        self.seats.pop(seat, None)
        self.pending = [c for c in self.pending if c.seat != seat]

    def on_violation(self, seat, why, now):
        self.n_violations += 1
        self._strike(seat, why, self.rt(now))

    def check_state(self, seat, data):
        """Range-check a SYNS before the relay forwards it. False = drop it."""
        try:
            f = S_SYNS.unpack(data)
        except struct.error:
            return False
        # f: 0 tag 1 ver 2 seat 3 flags 4 char 5 frame 6-8 pos 9-11 vel
        # 12 yaw 13 anim 14 cursor 15 state 16 step 17 invuln 18 power
        # 19 held 20 eaten_by 21 ack 22-23 sfx 24 reserved
        ok = (f[1] == 1 and abs(f[6]) <= POS_LIMIT and abs(f[7]) <= POS_LIMIT
              and abs(f[8]) <= POS_LIMIT and f[13] < 194
              and f[15] < N_STATES and f[17] <= 36
              and (f[19] <= SEATS or f[19] >= 0x20) and f[20] <= SEATS)
        if not ok:
            self.n_bad_states += 1
        return ok

    def on_state(self, seat, data, now):
        s = self.seats.get(seat)
        if s is None:
            return
        rt = self.rt(now)
        self.n_states += 1
        self._earn(s, len(data))
        f = S_SYNS.unpack(data)
        frame = f[5]
        s.last_heard = rt
        s.ack = f[21]
        last = s.newest()
        if last is not None:
            if frame <= last.frame:
                if last.frame - frame > RESET_BACK_FRAMES:
                    s.hist.clear()          # the console restarted
                    s.speed_win.clear()
                    last = None
                else:
                    return                  # late / duplicate: forward only
        smp = Sample()
        smp.frame = frame
        smp.rt = rt
        smp.x, smp.y, smp.z = f[6], f[7], f[8]
        smp.yaw = f[12]
        smp.state = f[15]
        smp.step = f[16]
        smp.invuln = f[17]
        smp.power = f[18]
        smp.flags = f[3]
        smp.held = f[19]
        smp.eaten_by = f[20]
        smp.look = f[4]
        smp.vy = f[10]
        s.hist.append(smp)
        if smp.look in (0, 1, 2) and frame > s.drop_frame:
            s.worn_look = smp.look      # VSREF3: a cap on its head, by its own word
            s.worn_rt = rt
        if last is not None and self.state == MS_RUNNING and seat in self.roster:
            self._speed_check(s, last, smp, rt)

    def check_object(self, seat, data):
        try:
            f = S_SYNO.unpack(data)
        except struct.error:
            return False
        # f: 0 tag 1 ver 2 seat 3 class 4 id 5 frame 6-8 pos 9-11 vel
        # 12 state 13 flags
        return (f[1] == 1 and f[3] == 1 and abs(f[6]) <= POS_LIMIT
                and abs(f[7]) <= POS_LIMIT and abs(f[8]) <= POS_LIMIT
                and f[12] in (1, 2, 3))

    def on_object(self, seat, data, now):
        f = S_SYNO.unpack(data)
        so = self.seats.get(seat)
        if so is not None:
            self._earn(so, len(data))
        star = self.stars.get(f[4])
        if star is None or star.state != ST_FREE:
            return
        # only the console that launched it speaks for its flight
        if star.owner != 0xFF and star.owner != seat:
            return
        star.x, star.y, star.z = f[6], f[7], f[8]
        star.known = True
        star.moving = f[12] == 1
        star.syno_rt = self.rt(now)
        s = self.seats.get(seat)
        if s is not None:
            s.last_heard = self.rt(now)

    def on_ping(self, seat, data, now):
        """The referee clock pong for a 20-byte 'SYNP' to 0xFE, or None."""
        s = self.seats.get(seat)
        if s is None:
            return None
        f = S_PING.unpack(data)
        if f[1] != 2 or f[3] != REF_MARK:
            return None
        rt = self.rt(now)
        self.n_pings += 1
        s.last_heard = rt
        self._earn(s, len(data))
        echo, held = f[6], f[7]
        if echo and echo in s.pongs:
            if not s.verified:
                s.verified = True
                s.credit = min(AMP_CREDIT_CAP, s.credit + SYNR_LEN)
                # proven: it hears the current state at once
                self._tx(seat, self._frame(rt, self._window(), seat=seat))
            if held != 0xFFFF:
                rtt = int(rt) - self.seat_unwire(seat, echo) - held
                if 0 <= rtt <= 5000:
                    s.rtts.append(rtt)
                    s.rtt_min = min(s.rtts)
        t_ref = self.ref_ms(now, seat)
        s.pongs.append(t_ref)
        return S_PING.pack(TAG_SYNQ, 2, REF_MARK, seat, 0, f[5], t_ref,
                           0xFFFF if s.rtt_min is None else
                           min(0xFFFE, s.rtt_min), s.cap())

    def on_claim(self, seat, data, now):
        s = self.seats.get(seat)
        if s is None:
            return
        rt = self.rt(now)
        self._earn(s, len(data))
        f = S_SYNE.unpack(data)
        if f[1] != 1:
            return
        kind, target, cid, aux, stamp, frame, ref_frame = (
            f[3], f[4], f[5], f[6], f[7], f[8], f[9])
        if not s.verified:
            # Not yet proven to receive at its address (see NO AMPLIFIER):
            # nothing it claims counts. A real console verifies within one
            # ping interval and keeps resending its claim until named.
            return
        if kind not in CLAIM_NAMES or cid == 0:
            self._strike(seat, "bad claim", rt)
            return
        s.last_heard = rt
        self.n_claims += 1
        # a resend: answer with the verdict again, never decide twice
        done = s.done.get(cid)
        if done is not None:
            self._resend_verdict(seat, done, rt)
            return
        if cid in s.pending_ids:
            return
        if kind == C_READY:
            self._ready(s, cid, target, aux, stamp, frame, rt)
            self._flush(rt)
            return
        c = Claim()
        c.seat, c.kind, c.target, c.cid, c.aux = seat, kind, target, cid, aux
        c.frame, c.ref_frame, c.arrival = frame, ref_frame, rt
        self.order_counter += 1
        c.order = self.order_counter
        live = self.state == MS_RUNNING or (
            self.state == MS_COUNTDOWN and kind == C_QUIT)
        if not live or seat not in self.roster:
            self._deny(c, D_STALE, rt)
            self._flush(rt)
            return
        # clamp the stamp into [arrival - cap, arrival]
        cap = s.cap()
        st = self.seat_unwire(seat, stamp)
        if st > rt + FUTURE_SLACK_MS:
            st = int(rt)
            s.clamps += 1
        elif st < rt - cap:
            st = int(rt - cap)
            s.clamps += 1
        c.stamp = st
        s.claims += 1
        if kind in (C_HURT_SELF, C_PIT, C_QUIT, C_SPIT, C_SWALLOW, C_ESCAPE,
                    C_THROW, C_SWING, C_BREAK, C_CAP_DROP):
            # nobody contends these: decide now
            self._decide(c, rt)
            self._flush(rt)
            return
        if len(s.pending_ids) >= PENDING_PER_SEAT:
            self._deny(c, D_STALE, rt)
            self._flush(rt)
            return
        others = [self.seats[o].cap() for o in self.roster
                  if o != seat and o in self.seats]
        c.resolve_at = c.stamp + min(HOLD_MAX_MS, max(others or [0]))
        s.pending_ids.add(cid)
        self.pending.append(c)

    # ---- the tick

    def tick(self, now):
        rt = self.rt(now)
        if self.state == MS_WAITING:
            self._maybe_start(rt)
        elif self.state == MS_COUNTDOWN and rt >= self.t0:
            self.state = MS_RUNNING
            if self._luigi():
                self._luigi_pick(rt, INF_PICK, self.t0)
        if self.pending:
            self._resolve(rt)
        if self.state == MS_RUNNING:
            self._presence(rt)
            self._hit_checks(rt)
            self._grab_checks(rt)
            self._cap_checks(rt)
            if self.match_ms and self.state == MS_RUNNING:
                end = self.t0 + self.match_ms
                if rt >= end + HOLD_MAX_MS:
                    self._resolve(rt)
                    self._over(rt, O_SURVIVORS_WIN if self._luigi()
                               else O_TIME_UP, end)
        self._flush(rt)
        self._catchup(rt)
        if rt - self.last_beat >= 1000.0 / HEARTBEAT_HZ:
            self.last_beat = rt
            if self.seats:
                self._broadcast(rt, self._window())

    def _maybe_start(self, rt):
        if self.cfg is None:
            return
        ready = [s for s in self.seats.values() if s.ready == self.cfg]
        cap = self.cfg[4]
        if len(ready) >= cap or (len(ready) >= 2 and self.first_ready
                                 is not None
                                 and rt - self.first_ready >= START_WAIT_MS):
            self._start(rt)
            self._flush(rt)

    def _resolve(self, rt):
        self.pending.sort(key=lambda c: (c.stamp, self.scores[c.seat],
                                         self.seat_rank[c.seat], c.order))
        while self.pending and self.pending[0].resolve_at <= rt:
            c = self.pending.pop(0)
            s = self.seats.get(c.seat)
            if s is not None:
                s.pending_ids.discard(c.cid)
            if self.state != MS_RUNNING:
                self._deny(c, D_STALE, rt)
                continue
            if c.seat not in self.roster:
                continue
            self._decide(c, rt)

    def _presence(self, rt):
        for seat in sorted(self.roster):
            s = self.seats.get(seat)
            if s is None:
                continue
            if rt - s.last_heard > PRESENCE_MS:
                self._drop_from_match(seat, rt, L_SILENT)
                if self.state != MS_RUNNING:
                    return

    def _hit_checks(self, rt):
        for s in list(self.seats.values()):     # a kick may drop a seat
            if s.hit_check is None or rt < s.hit_check[0]:
                continue
            verdict_rt = s.hit_check[1]
            s.hit_check = None
            took = any((x.flags & F_IN_HURT) or x.state in HURT_IDX
                       or x.invuln > 0 for x in s.hist if x.rt > verdict_rt)
            if not took and any(x.rt > verdict_rt for x in s.hist):
                self._strike(s.seat, "ignored a HIT verdict", rt)

    def _catchup(self, rt):
        """A seat whose ack trails what the 8-event window still carries gets
        the missed events directed to it (retained ones), or a SNAPSHOT."""
        if not self.newest_seq:
            return
        oldest_in_window = None
        win = list(self.events)[-SYNR_EVENTS:]
        if win:
            oldest_in_window = win[0][0]
        for seat, s in self.seats.items():
            if seat not in self.roster:
                continue
            behind = seq_after(s.ack, self.newest_seq)
            if behind <= SYNR_EVENTS or oldest_in_window is None:
                s.ack_lag_since = None
                continue
            if s.ack_lag_since is None:
                s.ack_lag_since = rt
            if rt - s.last_catchup < CATCHUP_EVERY_MS:
                continue
            s.last_catchup = rt
            retained = [(q, r) for q, r in self.events
                        if s.ack and 0 < seq_after(s.ack, q) <= behind]
            if s.ack and len(retained) == behind:
                for i in range(0, len(retained), SYNR_EVENTS):
                    chunk = retained[i:i + SYNR_EVENTS]
                    self._tx(seat, self._frame(
                        rt, [r for q, r in reversed(chunk)],
                        RF_DIRECTED | RF_RESEND, seat=seat))
            elif rt - s.ack_lag_since >= SNAPSHOT_LAG_MS or not s.ack:
                if rt - s.last_snapshot >= SNAPSHOT_LAG_MS:
                    self._snapshot(seat, rt)

    def _snapshot(self, seat, rt):
        s = self.seats.get(seat)
        if s is None:
            return
        s.last_snapshot = rt
        recs = []
        if self.state != MS_WAITING and self.cfg is not None:
            cfg = self.cfg
            recs.append(self._snap_record(
                EV_START, 0xFF, self.order_row, cfg[2], cfg[1],
                self._mask(self.roster), cfg[5], self.t0, self.seed, cfg[0],
                self._rules_word(cfg)))
            if self._luigi():
                recs.append(self._snap_record(
                    EV_TEAM_STATE, 0xFF, 1 if self.picked else 0, 0,
                    self._mask(self.luigis), self._mask(self.roster), 0,
                    self.t0))
        for inst in sorted(self.stars):
            st = self.stars[inst]
            recs.append(self._snap_record(
                EV_STAR_STATE, st.owner, inst, st.state, st.marker,
                st.star_id, 0, int(st.since), st.x, st.y, st.z))
        for cid in sorted(self.caps):
            cap = self.caps[cid]
            if cap.placed and cap.taken and rt < cap.avail_at:
                recs.append(self._snap_record(
                    EV_CAP_STATE, 0xFF, cid, 1, int(cap.avail_at - rt),
                    cap.look, 0, int(rt), cap.x, cap.y, cap.z))
            elif not cap.placed:
                recs.append(self._snap_record(
                    EV_CAP_STATE, 0xFF, cid, 2, int(max(0, cap.expires - rt)),
                    cap.look, 0, int(rt), cap.x, cap.y, cap.z))
        if self.over_event is not None:
            q, rec = self.over_event
            recs.append(rec[:0] + struct.pack("<H", 0) + rec[2:])
        parts = [recs[i:i + SYNR_EVENTS]
                 for i in range(0, len(recs), SYNR_EVENTS)] or [[]]
        n = min(15, len(parts))
        for i, chunk in enumerate(parts[:n]):
            self._tx(seat, self._frame(rt, chunk, RF_SNAPSHOT | RF_DIRECTED,
                                       ((i & 0xF) << 4) | n, seat=seat))

    # ---- READY

    def _ready(self, s, cid, target, aux, rules, digest, rt):
        first_to_n = rules & 0xFF
        mode = (rules >> 8) & 0xFF
        capacity = (rules >> 16) & 0xFF
        markers = (rules >> 24) & 0xFF
        c = Claim()
        c.seat, c.kind, c.target, c.cid, c.aux = s.seat, C_READY, target, cid, aux
        c.stamp = c.frame = c.ref_frame = 0
        c.arrival = rt
        if (target > 15 or aux > 3600 or first_to_n > 99 or not
                1 <= capacity <= SEATS or not 1 <= markers <= 8):
            self._deny(c, D_BAD, rt)
            return
        if mode not in MODES_ACCEPTED:
            self._deny(c, D_UNSUPPORTED, rt)
            return
        cfg = (target, aux, first_to_n, mode, capacity, markers, digest)
        if self.state == MS_OVER and cfg != self.cfg:
            if self.over_at is not None and rt - self.over_at < OVER_LINGER_MS:
                # the last match's MATCH_OVER keeps riding the window for
                # OVER_LINGER_MS (a console that lost it still gets it); this
                # READY gets no verdict yet, so its console keeps resending
                return
            self.reset_match(rt)        # a new match in the same session
        if self.cfg is None:
            self.cfg = cfg
            self.first_ready = rt
        if cfg != self.cfg:
            self._deny(c, D_MISMATCH, rt)
            return
        was = s.ready
        s.ready = cfg
        s.last_heard = rt
        self._accept(c, rt)
        if self.state == MS_WAITING:
            if was != cfg:
                self._emit(EV_JOIN, s.seat, 0, 0, 0, 0, 0, int(rt))
        elif self.state in (MS_COUNTDOWN, MS_RUNNING) and \
                s.seat not in self.roster:
            self.roster.add(s.seat)
            self.had = max(self.had, len(self.roster))
            self.scores[s.seat] = 0
            self._emit(EV_JOIN, s.seat, 1, 0, 0, 0, 0, int(rt))
            if self._luigi() and self.picked:
                # Tango 11-09: a player who comes in mid-match is a Luigi
                self._infect(s.seat, INF_LATE_JOIN, rt, int(rt))
            self._snapshot(s.seat, rt)

    # ---- verdicts

    def _record(self, c, ev):
        s = self.seats.get(c.seat)
        if s is None:
            return
        s.done[c.cid] = ev
        s.done_order.append(c.cid)
        while len(s.done_order) > DONE_PER_SEAT:
            s.done.pop(s.done_order.popleft(), None)
        self.n_verdicts += 1

    # DENY and ACCEPTED concern the claimant alone, so they are DIRECTED
    # records (seq 0) sent to it only, never part of the stream every seat
    # hears: a seat spamming bad claims cannot crowd the other seats' event
    # window or bandwidth. A resend of the claim gets the same record again.

    def _direct(self, c, rec, rt):
        self._record(c, (0, rec))
        self._tx(c.seat, self._frame(rt, [rec], RF_DIRECTED, seat=c.seat))

    def _deny(self, c, reason, rt, winner=0xFF, why=None):
        key = (c.kind, reason)
        self.deny_counts[key] = self.deny_counts.get(key, 0) + 1
        DENY_TOTALS[key] = DENY_TOTALS.get(key, 0) + 1
        # a refusal worth reading in the journal: what was refused, why and by
        # how much. LOST / GONE / STALE are ordinary contention and stay in the
        # counters. Rate-limited per session so a hostile seat cannot flood it.
        # FIX9C: a star or cap claim that was refused as STALE / GONE / LOST / BAD
        # used to leave no trace here, which is why "he picked up a star and got
        # nothing" could not be read back from the journal.
        loud = (c.kind in (C_GRAB, C_MARKER_BREAK, C_HIT, C_EAT) and
                reason in (D_RANGE, D_STATE, D_INVULN, D_TOO_OLD)) or (
                c.kind in (C_GRAB, C_CAP_TAKE, C_CAP_DROP) and
                reason in (D_STALE, D_GONE, D_LOST, D_BAD))
        if loud:
            if rt - self._dlog_t >= 1000:
                self._dlog_t, self._dlog_n = rt, 0
            if self._dlog_n < DENY_LOG_PER_S:
                self._dlog_n += 1
                self.log("arena %s deny %s seat %d target %d claim %d: %s%s"
                         % (self.code, CLAIM_NAMES.get(c.kind, c.kind),
                            c.seat, c.target, c.cid,
                            DENY_NAMES.get(reason, reason),
                            " (%s)" % why if why else ""))
        rec = self._snap_record(EV_DENY, c.seat, c.kind, reason, c.cid,
                                winner, c.target, int(rt))
        self._direct(c, rec, rt)
        s = self.seats.get(c.seat)
        if s is not None:
            s.pending_ids.discard(c.cid)

    def _accept(self, c, rt, spilled=0):
        rec = self._snap_record(EV_ACCEPTED, c.seat, c.kind, 0, c.cid,
                                spilled, 0, int(rt))
        self._direct(c, rec, rt)

    def _resend_verdict(self, seat, done, rt):
        seq, rec = done
        if seq and any(q == seq for q, r in list(self.events)[-SYNR_EVENTS:]):
            return      # still in the window every SYNR carries
        self._tx(seat, self._frame(rt, [rec], RF_DIRECTED | RF_RESEND,
                                   seat=seat))

    def _decide(self, c, rt):
        k = c.kind
        if (self.match_ms and k != C_QUIT
                and c.stamp >= self.t0 + self.match_ms):
            # touched after the clock ran out: the match ends at T0 +
            # match_ms even though claims are still decided during the hold
            self._deny(c, D_STALE, rt)
            return
        if self._luigi() and self._team_gate(c, rt):
            return
        if k == C_GRAB:
            self._grab(c, rt)
        elif k == C_MARKER_BREAK:
            self._marker(c, rt)
        elif k == C_HIT:
            self._hit(c, rt)
        elif k == C_HURT_SELF:
            inst = self._spill(c.seat, SP_HAZARD, rt)
            self.seats[c.seat].sanction_until = rt + 2000
            self._accept(c, rt, inst)
        elif k == C_PIT:
            inst = self._spill(c.seat, SP_PIT, rt) if c.aux & 1 else 0
            self.seats[c.seat].sanction_until = rt + 3000
            self._accept(c, rt, inst)
        elif k == C_QUIT:
            self._accept(c, rt)
            self._drop_from_match(c.seat, rt, L_QUIT)
        elif k == C_EAT:
            self._eat(c, rt)
        elif k in (C_SPIT, C_SWALLOW, C_ESCAPE):
            self._release(c, rt)
        elif k == C_CATCH:
            self._catch(c, rt)
        elif k in (C_THROW, C_SWING, C_BREAK):
            self._let_go(c, rt)
        elif k == C_STOMP:
            self._stomp(c, rt)
        elif k == C_CAP_TAKE:
            self._cap_take(c, rt)
        elif k == C_CAP_DROP:
            self._cap_drop(c, rt)
        elif k == C_TAG and self._luigi():
            self._tag(c, rt)
        else:
            self._deny(c, D_UNSUPPORTED, rt)

    @staticmethod
    def _attack_nc_ok(s, smp):
        """May an ATTACKER's sample carry the no_control flag?  Only while the
        ROM's own tongue holds it up (HITSPILL1): the sample is in an
        NC_ATTACK state, and the whole trailing run of no_control samples
        ending at it is in such states, is not hurt, began after a sample of
        the same seat that had the flag DOWN (a flag raised by a talk / cutscene
        / frozen state and carried into an attack state never qualifies), and
        is at most ATTACK_NC_MAX_FRAMES long (a state parked in the attack
        state with the flag up stops qualifying).  No pre-run sample on record
        (the first thing a seat ever sent) means no exemption.  The flag is the
        client's own report and a liar can leave it down, so this only
        decides which honest-looking samples a verdict may accept: it grants
        a cheater nothing it could not already do."""
        if smp.state not in NC_ATTACK_IDX or smp.state in DISABLED_IDX:
            return False
        hist = s.hist
        j = len(hist) - 1
        while j >= 0 and hist[j] is not smp:
            j -= 1
        if j < 0:
            return False
        first = smp
        while j >= 0 and hist[j].flags & F_NO_CONTROL:
            x = hist[j]
            if x.state not in NC_ATTACK_IDX or x.flags & F_IN_HURT:
                return False
            first = x
            j -= 1
        if j < 0:
            return False
        return smp.frame - first.frame <= ATTACK_NC_MAX_FRAMES

    def _usable(self, s, frame, attack=False, cap=False):
        """The claimant's own sample at `frame`, and whether it may act.
        attack=True: the sample is an attacker's, which may carry no_control
        for the length of a caught tongue (see _attack_nc_ok)."""
        smp = s.sample_at(frame)
        if smp is None:
            return None, D_STALE
        bad = F_IN_HURT | F_NO_CONTROL
        if attack and smp.flags & F_NO_CONTROL and self._attack_nc_ok(s, smp):
            bad = F_IN_HURT
        disabled = DISABLED_IDX
        if cap:
            # FIX9C: a cap that Yoshi's tongue pulled in is claimed from inside
            # the swallow (mIsNoControl up) and the put-on's own no-control
            # morph; a cap taken by touch is claimed from that morph too once
            # the sample is a frame late. Hurt, dead, eaten stay refused.
            bad = F_IN_HURT
            disabled = DISABLED_IDX - NOCONTROL_IDX
        if (smp.state in disabled or smp.flags & bad
                or smp.eaten_by or s.eaten_by is not None):
            return smp, D_STATE
        return smp, 0

    @staticmethod
    def _why_state(s, smp):
        """Journal words for a STATE refusal: what the claimant's sample says."""
        if smp is None:
            return "no sample near the claim frame"
        return ("state=%s flags=0x%02x eaten_by=%d%s" % (
            STATE_NAMES[smp.state] if smp.state < N_STATES else smp.state,
            smp.flags, smp.eaten_by,
            " referee-eaten" if s.eaten_by is not None else ""))

    @staticmethod
    def _in_flight(star, rt):
        """May the referee's copy of this star still be a trip or two behind a
        claimant's?  True while the owner's newest SYNO says it moves and is
        not stale (a star at rest, or one whose owner went quiet, is judged at
        the plain radius)."""
        return star.moving and rt - star.syno_rt <= FLIGHT_STALE_MS

    def _range_ok(self, c, smp, star, base_r, rt):
        """(ok, journal words): the claimant's sample vs the star, GRAB_R /
        MARKER_R plus FLIGHT_SLACK while the star is in flight."""
        if not star.known:
            return True, ""
        flying = self._in_flight(star, rt)
        limit = base_r + (FLIGHT_SLACK if flying else 0)
        d = _dist3(smp, star) / FIX
        if d > limit:
            return False, ("dist=%d limit=%d flying=%d star_age=%dms "
                           "syno_age=%dms" % (d, limit, 1 if flying else 0,
                                              rt - star.since,
                                              min(99999, rt - star.syno_rt)))
        return True, ""

    def _grab(self, c, rt):
        star = self.stars.get(c.target)
        if star is None or star.state != ST_FREE:
            t = self.taken.get(c.target)
            if t is not None:
                self._deny(c, D_LOST, rt, t[0])
            else:
                self._deny(c, D_GONE, rt)
            return
        s = self.seats[c.seat]
        smp, why = self._usable(s, c.frame)
        if why:
            self._deny(c, why, rt, why=self._why_state(s, smp)
                       if why == D_STATE else None)
            return
        ok, words = self._range_ok(c, smp, star, GRAB_R, rt)
        if not ok:
            self._deny(c, D_RANGE, rt, why=words)
            return
        del self.stars[c.target]
        self._taken(c.target, c.seat, c.stamp)
        self.scores[c.seat] = min(127, self.scores[c.seat] + 1)
        s.star_stamp = c.stamp
        self.progress = (self.progress + 1) & 0xFF
        ev = self._emit(EV_STAR_TAKEN, c.seat, c.target, self.scores[c.seat],
                        c.cid, 0, self.progress, c.stamp, smp.x, smp.y, smp.z)
        self._record(c, ev)
        self._wake_next(rt)
        n = self.cfg[2] if self.cfg else 0
        if n and self.scores[c.seat] >= n and not self._luigi():
            self._over(rt, O_FIRST_TO_N, c.stamp)

    def _taken(self, inst, seat, stamp):
        self.taken[inst] = (seat, stamp)
        self.taken_order.append(inst)
        while len(self.taken_order) > 32:
            old = self.taken_order.popleft()
            if old not in self.taken_order:
                self.taken.pop(old, None)

    def _marker(self, c, rt):
        star = self.stars.get(c.target)
        if star is None:
            self._deny(c, D_GONE, rt)
            return
        if star.state != ST_WAKING:
            b = self.broken.get(c.target)
            if b is not None and b != c.seat:
                self._deny(c, D_LOST, rt, b)
            else:
                self._deny(c, D_GONE, rt)
            return
        s = self.seats[c.seat]
        smp, why = self._usable(s, c.frame)
        if why:
            self._deny(c, why, rt, why=self._why_state(s, smp)
                       if why == D_STATE else None)
            return
        ok, words = self._range_ok(c, smp, star, MARKER_R, rt)
        if not ok:
            self._deny(c, D_RANGE, rt, why=words)
            return
        star.state = ST_FREE
        star.owner = c.seat
        star.x, star.y, star.z = smp.x, smp.y, smp.z
        star.known = False      # it pops and flies: SYNO frames place it
        star.since = rt
        self.broken[c.target] = c.seat
        ev = self._emit(EV_MARKER_BROKEN, c.seat, c.target, star.star_id,
                        c.cid, c.aux, 0, c.stamp, smp.x, smp.y, smp.z)
        self._record(c, ev)

    def _hit(self, c, rt, eat=False):
        victim = c.target
        if victim == c.seat or victim not in self.roster or \
                victim not in self.seats:
            self._deny(c, D_SELF, rt)
            return
        a = self.seats[c.seat]
        v = self.seats[victim]
        att = a.sample_at(c.frame)
        if att is None:
            self._deny(c, D_STALE, rt)
            return
        trade = False
        if a.last_hit is not None:
            dt = c.stamp - a.last_hit
            if eat:
                # A tongue never trades with a hit (FIELD4 F9-a): an eater that
                # was struck inside the window loses its catch, so exactly one
                # of the two lands. (Before this an EAT stamped within
                # TRADE_MS of the hit that struck its eater rode the HIT-vs-HIT
                # trade rule: the victim was eaten AND the eater lost a star.)
                if dt < HIT_IMMUNE_MS:
                    self._deny(c, D_STATE, rt, why="the eater was hit %d ms "
                               "before its tongue" % dt)
                    return
            elif abs(dt) <= TRADE_MS and a.last_hit_by == victim:
                trade = True
            elif 0 <= dt < HIT_IMMUNE_MS:
                self._deny(c, D_STATE, rt)
                return
        if not eat and self._victim_catching(v, c, rt):
            # The ROM's tongue that has caught somebody (or is catching one
            # at this very instant) has Player_DisableInteraction up: the
            # pound / punch bounces off. Ties go to the tongue.
            self._deny(c, D_INVULN, rt, why="the victim is eating or catching "
                       "(an EAT within %d ms)" % TRADE_MS)
            return
        if not trade:
            smp, why = self._usable(a, c.frame, attack=True)
            if why:
                self._deny(c, why, rt, why=self._why_state(a, smp)
                           if why == D_STATE else None)
                return
        want = YOSHI_POWER_IDX if eat else ATTACK_IDX
        win = a.samples_between(c.frame - ATTACK_LOOKBACK_FRAMES, c.frame)
        if not any(x.state in want for x in win):
            self._deny(c, D_STATE, rt)
            return
        vs = v.sample_at(c.ref_frame)
        if vs is None:
            self._deny(c, D_STALE, rt)
            return
        # how long before the touch the referee itself received the frame
        # the attacker drew: the victim's self-reported RTT plays no part
        # (REVREFEREE1 F1: a victim inflating it voided every hit on it)
        age = c.stamp - vs.rt
        if age > COMP_MAX_MS + FRAME_MS:
            self._deny(c, D_TOO_OLD, rt)
            return
        dh = math.hypot(att.x - vs.x, att.z - vs.z)
        if dh > REACH_H * FIX or abs(att.y - vs.y) > REACH_V * FIX:
            self._deny(c, D_RANGE, rt)
            return
        if not eat and self._luigi() and c.seat in self.luigis:
            self._infect_claim(c, victim, INF_HIT, att, vs, rt)
            return
        if (vs.state in DISABLED_IDX or vs.flags & (F_IN_HURT | F_NO_CONTROL)
                or vs.invuln > 0 or vs.power & (P_MEGA | P_VANISH | P_METAL)
                or vs.eaten_by or v.eaten_by is not None):
            self._deny(c, D_INVULN, rt)
            return
        if v.last_hit is not None and c.stamp - v.last_hit < HIT_IMMUNE_MS:
            self._deny(c, D_INVULN, rt)
            return
        if (v.star_stamp is not None
                and 0 <= c.stamp - v.star_stamp < STAR_POSE_MS):
            # FIX9C (Tango's "picked up a star and it said 0"): the victim
            # touched a star BEFORE this hit in the referee's time and is still
            # in the ROM's star-get pose, which is a no-control hold nothing
            # hurts. The attacker's screen shows the victim a few frames late
            # (still walking), so the sample at ref_frame cannot tell; the
            # grab's own stamp can.
            self._deny(c, D_INVULN, rt, why="the victim is in its star-get "
                       "pose (%d ms after its star)" % (c.stamp - v.star_stamp))
            return
        if eat:
            v.eaten_by = c.seat
            v.eaten_at = rt
            a.eat_stamp = c.stamp
            a.eat_target = victim
            v.sanction_until = rt + 60000
            ev = self._emit(EV_EATEN, victim, c.seat, 0, c.cid, 0, 0, c.stamp,
                            att.x, att.y, att.z)
            self._record(c, ev)
            return
        if vs.state in CLASH_IDX and _facing(vs, att):
            ev = self._emit(EV_CLASH, victim, c.seat, 0, c.cid, 0, 0, c.stamp,
                            att.x, att.y, att.z)
            self._record(c, ev)
            return
        v.last_hit = c.stamp
        v.last_hit_by = c.seat
        v.sanction_until = rt + 2000
        v.hit_check = (rt + v.one_way() * 2 + INVULN_CHEAT_MS, rt)
        inst = self._spill(victim, SP_HIT, rt, emit=False)
        ev = self._emit(EV_HIT, victim, c.seat, 1 if trade else 0, c.cid,
                        c.aux, inst, c.stamp, att.x, att.y, att.z)
        self._record(c, ev)
        if inst:
            self._emit_spill(inst, victim, SP_HIT, rt)

    def _victim_catching(self, v, c, rt):
        """Is the victim of HIT claim `c` an eater right now: it holds a
        mouthful, won an EAT of the attacker within TRADE_MS of this claim's
        stamp, or has such an EAT of its own waiting to be decided?"""
        if any(x.eaten_by == v.seat for x in self.seats.values()):
            return True
        if v.eat_target == c.seat and v.eat_stamp is not None and                 abs(c.stamp - v.eat_stamp) <= TRADE_MS:
            return True
        for p in self.pending:
            if (p.kind == C_EAT and p.seat == v.seat and p.target == c.seat
                    and abs(c.stamp - p.stamp) <= TRADE_MS
                    # a pending claim is unchecked: it shields only if the
                    # eater's own samples show the tongue out around its frame
                    # (the test _hit applies when it decides it), so a seat
                    # cannot buy immunity by spamming dummy EAT claims
                    and any(x.state in YOSHI_POWER_IDX for x in v.samples_between(
                        p.frame - ATTACK_LOOKBACK_FRAMES, p.frame))):
                return True
        return False

    def _eat(self, c, rt):
        if self.seats[c.seat].eaten_by is not None:
            self._deny(c, D_STATE, rt)
            return
        if any(s.eaten_by == c.seat for s in self.seats.values()):
            self._deny(c, D_STATE, rt)      # one mouthful at a time
            return
        self._hit(c, rt, eat=True)

    def _release(self, c, rt):
        if c.kind == C_ESCAPE:
            victim, eater = c.seat, c.target
        else:
            victim, eater = c.target, c.seat
        v = self.seats.get(victim)
        if v is None or v.eaten_by != eater:
            self._deny(c, D_GONE, rt)
            return
        if c.kind == C_ESCAPE and rt - v.eaten_at < ESCAPE_MIN_MS:
            self._deny(c, D_STATE, rt)      # too soon after the EATEN to be a real escape
            return
        v.eaten_by = None
        v.sanction_until = rt + 2000
        how = {C_SPIT: 1, C_SWALLOW: 2, C_ESCAPE: 3}[c.kind]
        ev = self._emit(EV_RELEASED, victim, eater, how, c.cid, 0, 0, int(rt))
        self._record(c, ev)

    # ---- GRAB1: stomping a player (F9-d)

    def _stomp(self, c, rt):
        """A claimant fell onto a player.  The ROM (func_ov002_020d869c, the
        no-attack-flag branch, on the VICTIM): not eaten, body collision on,
        not no-control; JumpedOnByPlayer (the stomper falling, above the victim,
        in contact); the stomper airborne and not ground-pounding (func_ov002_
        020d9298 bounces it); then the victim squashes (func_ov002_020db8bc).
        No hurt, no star, no invincibility check (the ROM has none here)."""
        victim = c.target
        if victim == c.seat or victim not in self.roster or                 victim not in self.seats:
            self._deny(c, D_SELF, rt)
            return
        a = self.seats[c.seat]
        v = self.seats[victim]
        if a.grabbed_by is not None or v.grabbed_by is not None or                 v.eaten_by is not None:
            self._deny(c, D_STATE, rt)
            return
        last = a.last_stomp.get(victim)
        if last is not None and 0 <= rt - last < STOMP_MIN_MS:
            self._deny(c, D_STATE, rt)
            return
        att = a.sample_at(c.frame)
        if att is None:
            self._deny(c, D_STALE, rt)
            return
        smp, why = self._usable(a, c.frame)
        if why:
            self._deny(c, why, rt)
            return
        win = a.samples_between(c.frame - 3, c.frame)
        if att.flags & F_GROUNDED or att.state in GROUND_POUND_IDX or                 not any(x.vy < 0 for x in win):
            self._deny(c, D_STATE, rt)          # not falling onto anyone
            return
        vs = v.sample_at(c.ref_frame)
        if vs is None:
            self._deny(c, D_STALE, rt)
            return
        if c.stamp - vs.rt > COMP_MAX_MS + FRAME_MS:
            self._deny(c, D_TOO_OLD, rt)
            return
        dh = math.hypot(att.x - vs.x, att.z - vs.z)
        dy = att.y - vs.y
        if dh > STOMP_REACH_H * FIX or dy > REACH_V * FIX or                 dy < STOMP_ABOVE_MIN * FIX:
            self._deny(c, D_RANGE, rt)
            return
        if self._luigi() and c.seat in self.luigis:
            a.last_stomp[victim] = rt
            self._infect_claim(c, victim, INF_STOMP, att, vs, rt)
            return
        if vs.flags & F_NO_CONTROL or vs.eaten_by or vs.state in GRABBED_IDX:
            self._deny(c, D_INVULN, rt)
            return
        a.last_stomp[victim] = rt
        ev = self._emit(EV_STOMPED, victim, c.seat, 0, c.cid, 0, 0, c.stamp,
                        att.x, att.y, att.z)
        self._record(c, ev)

    # ---- GRAB1: the Wario hold (catch, swing, throw, break)

    def _catch(self, c, rt):
        """A Wario in a punch or a dive caught a player in the stun a Wario hit
        leaves.  The ROM's own gate (func_ov002_020d85fc): the victim in St_Hurt
        (phase 2, mStateTimer set, actor flag 0x80) or St_DizzyStars, the
        attacker's param1 2, Player::TryGrab(attacker, victim) (attacker in a
        punch / dive / swim state, holding nothing).  The victim's own console
        runs the last two checks again on the GRABBED event."""
        victim = c.target
        if victim == c.seat or victim not in self.roster or \
                victim not in self.seats:
            self._deny(c, D_SELF, rt)
            return
        a = self.seats[c.seat]
        v = self.seats[victim]
        if (a.holding is not None or a.grabbed_by is not None
                or a.eaten_by is not None or v.grabbed_by is not None
                or v.eaten_by is not None or v.holding is not None
                or any(x.eaten_by == c.seat for x in self.seats.values())):
            self._deny(c, D_STATE, rt)
            return
        att = a.sample_at(c.frame)
        if att is None:
            self._deny(c, D_STALE, rt)
            return
        smp, why = self._usable(a, c.frame, attack=True)
        if why:
            self._deny(c, why, rt)
            return
        if att.look != LOOK_WARIO:
            self._deny(c, D_STATE, rt)
            return
        win = a.samples_between(c.frame - ATTACK_LOOKBACK_FRAMES, c.frame)
        if not any(x.state in CATCH_ATTACK_IDX for x in win):
            self._deny(c, D_STATE, rt)
            return
        vs = v.sample_at(c.ref_frame)
        if vs is None:
            self._deny(c, D_STALE, rt)
            return
        if c.stamp - vs.rt > COMP_MAX_MS + FRAME_MS:
            self._deny(c, D_TOO_OLD, rt)
            return
        dh = math.hypot(att.x - vs.x, att.z - vs.z)
        if dh > REACH_H * FIX or abs(att.y - vs.y) > REACH_V * FIX:
            self._deny(c, D_RANGE, rt)
            return
        if vs.state not in STUN_IDX or vs.eaten_by or \
                vs.power & (P_MEGA | P_VANISH | P_METAL):
            self._deny(c, D_INVULN, rt)
            return
        if vs.state in HURT_STUN_IDX and (
                v.last_hit is None or v.last_hit_by != c.seat
                or not 0 <= c.stamp - v.last_hit <= GRAB_STUN_MAX_MS):
            # the stun is the one a hit by THIS Wario leaves
            self._deny(c, D_INVULN, rt)
            return
        v.grabbed_by = c.seat
        v.grabbed_at = rt
        a.holding = victim
        v.sanction_until = rt + 2000
        ev = self._emit(EV_GRABBED, victim, c.seat, 0, c.cid, 0, 0, c.stamp,
                        att.x, att.y, att.z)
        self._record(c, ev)

    def _let_go(self, c, rt):
        """THROW / SWING (the holder) and BREAK (the victim) end a hold."""
        if c.kind == C_BREAK:
            victim, holder = c.seat, c.target
        else:
            victim, holder = c.target, c.seat
        v = self.seats.get(victim)
        h = self.seats.get(holder)
        if v is None or h is None or v.grabbed_by != holder:
            self._deny(c, D_GONE, rt)
            return
        if rt - v.grabbed_at < GRAB_RELEASE_MIN_MS:
            self._deny(c, D_STATE, rt)          # too soon to be a real throw / struggle
            return
        speed, angle = 0, 0
        if c.kind != C_BREAK:
            speed, angle = c.ref_frame, c.aux
            if speed > GRAB_SPEED_MAX:
                self._deny(c, D_BAD, rt)
                return
        v.grabbed_by = None
        h.holding = None
        v.sanction_until = rt + 2000
        how = {C_THROW: 1, C_SWING: 2, C_BREAK: 3}[c.kind]
        ev = self._emit(EV_THROWN, victim, holder, how, c.cid, angle, 0,
                        int(rt), speed, 0, 0)
        self._record(c, ev)

    def _end_hold(self, victim, rt):
        """A hold that cannot go on (a leave, a console that never showed it):
        let the victim go, softly, and tell everyone."""
        v = self.seats.get(victim)
        if v is None or v.grabbed_by is None:
            return
        holder = v.grabbed_by
        v.grabbed_by = None
        v.sanction_until = rt + 2000
        h = self.seats.get(holder)
        if h is not None:
            h.holding = None
        self._emit(EV_THROWN, victim, holder, 4, 0, 0, 0, int(rt), 0, 0, 0)

    def _grab_checks(self, rt):
        """The failsafe: a hold whose victim does not report St_Grabbed (or
        whose holder does not report a holding state) for GRAB_STUCK_MS is
        let go, so no console is left parked in a state the other never saw."""
        for v in list(self.seats.values()):
            if v.grabbed_by is None or rt - v.grabbed_at < GRAB_STUCK_MS:
                continue
            h = self.seats.get(v.grabbed_by)
            vn, hn = v.newest(), (h.newest() if h is not None else None)
            if (vn is None or vn.state not in GRABBED_IDX
                    or hn is None or hn.state not in HOLDER_IDX
                    or rt - v.last_heard > GRAB_STUCK_MS
                    or (h is not None and rt - h.last_heard > GRAB_STUCK_MS)):
                self.log("arena %s hold of seat %d by seat %d dropped: not "
                         "reported" % (self.code, v.seat, v.grabbed_by))
                self._end_hold(v.seat, rt)

    # ---- VSREF3: caps

    def _cap_take(self, c, rt):
        """A claimant's body touched a cap and its ROM pickup started.  The
        earliest stamp wins (the pending queue orders them); a cap that is not
        there, is taken, is out of reach or whose place hash differs is refused,
        and so is a claimant that may not act."""
        cid = c.target
        if cid == 0:
            self._deny(c, D_BAD, rt)
            return
        cap = self.caps.get(cid)
        if cap is None:
            if cid <= CAP_PLACED_MAX and not CAP_MAPS.get(
                    self.cfg[0] if self.cfg else -1):
                # a map with no table: the first claim names the cap
                cap = Cap(cid, True, rt)
                cap.hash = c.aux
                cap.look = LOOK_NONE
                self.caps[cid] = cap
            else:
                self._deny(c, D_GONE, rt)
                return
        if cap.placed and cap.known and c.aux != cap.hash:
            self._deny(c, D_BAD, rt)
            return
        if cap.taken and c.stamp >= cap.avail_at:
            cap.taken = False                   # it has come back
        if cap.taken:
            if c.stamp - cap.taken_stamp < 1000:
                self._deny(c, D_LOST, rt, cap.taken_by)
            else:
                self._deny(c, D_GONE, rt)
            return
        s = self.seats[c.seat]
        smp, why = self._usable(s, c.frame, cap=True)
        if why:
            self._deny(c, why, rt, why=self._why_state(s, smp)
                       if why == D_STATE else None)
            return
        if cap.known:
            limit = CAP_R
            if not cap.placed and rt - cap.born <= CAP_FLIGHT_MS:
                limit += CAP_FLIGHT_SLACK
            d = _dist3(smp, cap) / FIX
            if d > limit:
                self._deny(c, D_RANGE, rt, why="cap %d dist=%d limit=%d"
                           % (cid, d, limit))
                return
        cap.taken = True
        cap.taken_by = c.seat
        cap.taken_stamp = c.stamp
        respawn = 0
        if cap.placed:
            cap.avail_at = c.stamp + CAP_RESPAWN_MS
            respawn = CAP_RESPAWN_MS
        else:
            del self.caps[cid]
        if cap.look in (0, 1, 2):
            s.worn_look, s.worn_rt = cap.look, rt
        ev = self._emit(EV_CAP_TAKEN, c.seat, cid, cap.look, c.cid, respawn,
                        0, c.stamp, smp.x, smp.y, smp.z)
        self._record(c, ev)

    def _cap_hurt_evidence(self, s, rt):
        """Did this seat's own SYNS show a hurt lately?"""
        for x in s.hist:
            if rt - x.rt <= CAP_HURT_EVIDENCE_MS and (
                    x.flags & F_IN_HURT or x.state in CAP_HURT_IDX):
                return True
        return False

    def _cap_drop(self, c, rt, final=False):
        """The ROM popped a hurt seat's cap (St_Hurt_Init).  Granted only to a
        seat that wore that cap by its own word, was seen hurt, and has not
        dropped one in the last CAP_DROP_MIN_MS; the pop point is the referee's
        own sample of the seat, the angles are the claim's."""
        s = self.seats[c.seat]
        look = c.target
        if look not in (0, 1, 2):
            self._deny(c, D_BAD, rt)
            return
        if rt - s.drop_rt < CAP_DROP_MIN_MS:
            self._deny(c, D_STATE, rt, why="a cap came off %d ms ago"
                       % (rt - s.drop_rt))
            return
        if s.worn_look != look or rt - s.worn_rt > CAP_WORN_MS:
            self._deny(c, D_STATE, rt, why="that cap is not on this seat's head")
            return
        if not self._cap_hurt_evidence(s, rt):
            if not final:
                self.cap_wait.append((rt + CAP_DROP_WAIT_MS, c))
                s.pending_ids.add(c.cid)
                return
            self._deny(c, D_STATE, rt, why="not hurt")
            return
        smp = s.sample_at(c.frame) or s.newest()
        if smp is None:
            self._deny(c, D_STALE, rt)
            return
        cid = self._alloc_cap()
        cap = Cap(cid, False, rt)
        cap.x, cap.y, cap.z = smp.x, smp.y + CAP_POP_UP, smp.z
        cap.known = True
        cap.look = look
        cap.expires = rt + CAP_LIFE_MS
        self.caps[cid] = cap
        s.worn_look, s.worn_rt = None, -1e9
        s.drop_frame = c.frame
        s.drop_rt = rt
        ev = self._emit(EV_CAP_DROPPED, c.seat, cid, look, c.cid, c.aux,
                        c.ref_frame & 0xFFFF, int(rt), cap.x, cap.y, cap.z)
        self._record(c, ev)

    def _alloc_cap(self):
        for _ in range(256 - CAP_DROP_BASE):
            cid = self.next_cap
            self.next_cap += 1
            if self.next_cap > 255:
                self.next_cap = CAP_DROP_BASE
            if cid not in self.caps:
                return cid
        # all taken (cannot happen at 7 s a life): the oldest goes
        old = min((k for k in self.caps if k >= CAP_DROP_BASE),
                  key=lambda k: self.caps[k].born)
        del self.caps[old]
        return old

    def _cap_checks(self, rt):
        """Knocked-off caps run out; a CAP_DROP waiting for its evidence is
        decided when the wait is over."""
        for cid in [k for k, v in self.caps.items()
                    if not v.placed and rt >= v.expires]:
            del self.caps[cid]
        if self.cap_wait:
            ready = [w for w in self.cap_wait if w[0] <= rt]
            self.cap_wait = [w for w in self.cap_wait if w[0] > rt]
            for _, c in ready:
                s = self.seats.get(c.seat)
                if s is None:
                    continue
                s.pending_ids.discard(c.cid)
                if c.seat not in self.roster:
                    continue
                if self.state == MS_RUNNING:
                    self._cap_drop(c, rt, final=True)
                else:
                    self._deny(c, D_STALE, rt)

    # ---- stars

    def _alloc_inst(self):
        for _ in range(255):
            inst = self.next_inst
            self.next_inst = self.next_inst % 255 + 1
            if inst not in self.stars:
                self.taken.pop(inst, None)
                self.broken.pop(inst, None)
                return inst
        return 0

    def _wake_next(self, rt):
        if self.cfg is None:
            return
        p = self.progress
        star_id = STAR_ORDERS[self.order_row][p] if p < 12 else 0
        if star_id >= self.cfg[5]:
            return          # no marker with that id on this map (ROM: none)
        if any(st.star_id == star_id and st.state == ST_WAKING
               for st in self.stars.values()):
            return
        inst = self._alloc_inst()
        if not inst:
            return
        self.stars[inst] = Star(inst, ST_WAKING, star_id, star_id, rt)
        self._emit(EV_STAR_WAKE, 0xFF, inst, star_id, p, 0, 0, int(rt))

    def _free_marker(self):
        busy = set(st.marker for st in self.stars.values())
        n = self.cfg[5] if self.cfg else 1
        free = [m for m in range(n) if m not in busy]
        return self.rng.choice(free) if free else self.rng.randrange(n)

    def _spill(self, seat, cause, rt, emit=True):
        """One star leaves `seat` if it has any: a new FREE instance flying
        from the seat's position to a free marker. Returns the instance."""
        if self.scores[seat] <= 0:
            return 0
        inst = self._alloc_inst()
        if not inst:
            return 0
        self.scores[seat] -= 1
        st = Star(inst, ST_FREE, self._free_marker(), 0xFF, rt)
        s = self.seats.get(seat)
        last = s.newest() if s is not None else None
        if last is not None:
            st.x, st.y, st.z = last.x, last.y, last.z
        st.owner = seat if cause != SP_LEAVE else 0xFF
        st.known = False        # in flight to its marker: no range check
        self.stars[inst] = st
        if emit:
            self._emit_spill(inst, seat, cause, rt)
        return inst

    def _emit_spill(self, inst, seat, cause, rt):
        st = self.stars[inst]
        self._emit(EV_SPILL, seat, inst, st.marker, cause, self.scores[seat],
                   0, int(rt), st.x, st.y, st.z)

    # ---- LUIGI INFECTION (mode 2, lane REFLUIGI1)

    def _luigi(self):
        return self.cfg is not None and self.cfg[3] == MODE_LUIGI

    def _survivors(self):
        return self.roster - self.luigis

    def _team_gate(self, c, rt):
        """The team rules that refuse a claim before its kind is judged.
        True = refused (the DENY is sent)."""
        k = c.kind
        me_luigi = c.seat in self.luigis
        if k in (C_HIT, C_STOMP, C_EAT, C_CATCH, C_TAG):
            if c.target in self.luigis and c.target != c.seat:
                # a Luigi is immune to everything, from anyone
                self._deny(c, D_INVULN, rt, why="the victim is a Luigi")
                return True
        if me_luigi and k in (C_EAT, C_CATCH, C_CAP_TAKE, C_CAP_DROP):
            self._deny(c, D_TEAM, rt)
            return True
        if not me_luigi and k == C_TAG:
            self._deny(c, D_TEAM, rt)
            return True
        if k == C_CAP_TAKE and not me_luigi:
            cap = self.caps.get(c.target)
            if cap is not None and cap.look == LOOK_LUIGI:
                self._deny(c, D_TEAM, rt, why="the Luigi cap")
                return True
        return False

    def _tag(self, c, rt):
        """A Luigi's body touched a survivor's (Classic's guard runs on any
        player contact): body-contact reach, the usual lag checks."""
        victim = c.target
        if victim == c.seat or victim not in self.roster or \
                victim not in self.seats:
            self._deny(c, D_SELF, rt)
            return
        a = self.seats[c.seat]
        v = self.seats[victim]
        att, why = self._usable(a, c.frame)
        if why:
            self._deny(c, why, rt, why=self._why_state(a, att)
                       if why == D_STATE else None)
            return
        vs = v.sample_at(c.ref_frame)
        if vs is None:
            self._deny(c, D_STALE, rt)
            return
        if c.stamp - vs.rt > COMP_MAX_MS + FRAME_MS:
            self._deny(c, D_TOO_OLD, rt)
            return
        dh = math.hypot(att.x - vs.x, att.z - vs.z)
        if dh > TAG_REACH_H * FIX or abs(att.y - vs.y) > TAG_REACH_V * FIX:
            self._deny(c, D_RANGE, rt)
            return
        self._infect_claim(c, victim, INF_TAG, att, vs, rt)

    def _infect_claim(self, c, victim, cause, att, vs, rt):
        """A Luigi's HIT / STOMP / TAG on a survivor that passed its checks.
        Classic's guard (unmatched/LuigiInfection_Hit.cpp) runs before the
        ROM's own resolution, so the survivor's invincibility, power and hit
        immunity do not stop it; a survivor in a mouth or a hold is not in
        the arena to touch."""
        v = self.seats[victim]
        if (v.eaten_by is not None or vs.eaten_by or v.grabbed_by is not None
                or victim in self.luigis):
            self._deny(c, D_INVULN, rt)
            return
        ev = self._infect(victim, cause, rt, c.stamp, by=c.seat, cid=c.cid,
                          pos=(att.x, att.y, att.z))
        self._record(c, ev)

    def _infect(self, seat, cause, rt, t, by=0xFF, cid=0, pos=(0, 0, 0)):
        """`seat` is a Luigi now: end any hold or mouthful it is part of,
        tell everyone (INFECTED), then see whether the match is decided."""
        s = self.seats.get(seat)
        if s is not None:
            if s.grabbed_by is not None:
                self._end_hold(seat, rt)
            if s.holding is not None:
                self._end_hold(s.holding, rt)
            if s.eaten_by is not None:
                eater = s.eaten_by
                s.eaten_by = None
                self._emit(EV_RELEASED, seat, eater, 4, 0, 0, 0, int(rt))
        for o in self.seats.values():
            if o.eaten_by == seat:
                o.eaten_by = None
                self._emit(EV_RELEASED, o.seat, seat, 4, 0, 0, 0, int(rt))
        self.luigis.add(seat)
        self.picked = True
        ev = self._emit(EV_INFECTED, seat, by, cause, cid,
                        self._mask(self.luigis), len(self._survivors()), t,
                        pos[0], pos[1], pos[2])
        self.log("arena %s INFECTED seat %d by %s cause %d (luigis %s, "
                 "survivors %s)" % (self.code, seat,
                                     "ref" if by == 0xFF else by, cause,
                                     sorted(self.luigis),
                                     sorted(self._survivors())))
        if self.state == MS_RUNNING and self.roster and \
                not self._survivors():
            self._over(rt, O_LUIGIS_WIN, t)
        return ev

    def _luigi_pick(self, rt, cause, t):
        """Classic's rule (port_luigi_seed / li_hand_off): the (seed mod n)-th
        player in the match, n = the players in the match, seat order."""
        seats = sorted(self.roster)
        if not seats:
            return
        self._infect(seats[self.seed % len(seats)], cause, rt, t)

    def _luigi_after_leave(self, rt):
        """The match after a leave, mode 2 (roster not empty)."""
        if self.had >= 2 and len(self.roster) == 1:
            last = next(iter(self.roster))
            self._over(rt, O_LUIGIS_WIN if last in self.luigis
                       else O_SURVIVORS_WIN)
        elif self.picked and not self._survivors():
            self._over(rt, O_LUIGIS_WIN)
        elif self.picked and not (self.roster & self.luigis) and \
                self.state == MS_RUNNING:
            self._luigi_pick(rt, INF_HANDOFF, int(rt))

    # ---- leaving

    def _drop_from_match(self, seat, rt, why):
        if seat not in self.roster:
            return
        n = 0
        if self.state in (MS_COUNTDOWN, MS_RUNNING):
            while self.scores[seat] > 0:
                if not self._spill(seat, SP_LEAVE, rt):
                    self.scores[seat] = 0
                    break
                n += 1
        for s in list(self.seats.values()):
            if s.grabbed_by == seat or s.seat == seat:
                self._end_hold(s.seat, rt)
        self.roster.discard(seat)
        self.luigis.discard(seat)
        self._emit(EV_LEAVE, seat, why, 0, 0, n, 0, int(rt))
        for s in self.seats.values():
            if s.eaten_by == seat:
                s.eaten_by = None
                self._emit(EV_RELEASED, s.seat, seat, 4, 0, 0, 0, int(rt))
        me = self.seats.get(seat)
        if me is not None:
            me.ready = None
            if me.eaten_by is not None:
                eater = me.eaten_by
                me.eaten_by = None
                self._emit(EV_RELEASED, seat, eater, 4, 0, 0, 0, int(rt))
        self.pending = [c for c in self.pending if c.seat != seat]
        self.log("arena %s seat %d left the match (%d), %d stars spilled"
                 % (self.code, seat, why, n))
        if self.state == MS_RUNNING or self.state == MS_COUNTDOWN:
            if not self.roster:
                self._over(rt, O_EVERYONE_LEFT)
            elif self._luigi():
                self._luigi_after_leave(rt)
            elif self.had >= 2 and len(self.roster) == 1:
                self._over(rt, O_LAST_STANDING)

    # ---- strikes (phase A: logged; kick is phase B)

    def _strike(self, seat, why, rt):
        s = self.seats.get(seat)
        if s is None:
            return
        s.strikes.append(rt)
        s.strikes_total += 1
        while s.strikes and rt - s.strikes[0] > STRIKE_WINDOW_MS:
            s.strikes.popleft()
        if rt - s.speed_logged >= 1000:
            s.speed_logged = rt
            self.log("arena %s strike seat %d: %s (%d in 10 s, %d total)"
                     % (self.code, seat, why, len(s.strikes),
                        s.strikes_total))
        if self.kick_on and len(s.strikes) >= STRIKES_TO_KICK:
            self.log("arena %s KICK seat %d" % (self.code, seat))
            self.kick(seat)

    def _speed_check(self, s, a, b, rt):
        df = b.frame - a.frame
        if df <= 0 or df > 30:
            s.speed_win.clear()
            return
        if (rt < s.sanction_until or b.eaten_by or a.eaten_by
                or s.grabbed_by is not None):
            s.speed_win.clear()
            return
        step = math.hypot(b.x - a.x, b.z - a.z) / FIX / df
        if b.flags & F_TELEPORT:
            self._strike(s.seat, "unsanctioned teleport", rt)
            s.speed_win.clear()
            return
        cls = SPEED_CLASS[b.state]
        if cls is None:
            s.speed_win.clear()
            return
        if step > SPEED_LIMITS[cls]:
            self._strike(s.seat, "speed %.0f/frame in %s"
                         % (step, STATE_NAMES[b.state]), rt)
        s.speed_win.append((step, cls))
        if len(s.speed_win) == SPEED_AVG_FRAMES and \
                all(k != "slide" for st, k in s.speed_win):
            avg = sum(st for st, k in s.speed_win) / SPEED_AVG_FRAMES
            if avg > SPEED_AVG_LIMIT:
                self._strike(s.seat, "average speed %.0f/frame" % avg, rt)
                s.speed_win.clear()

    # ---- introspection (tests, stats)

    def invariants(self):
        """Bounded tables and a sane ledger; [] when every check holds."""
        bad = []
        if any(not -128 <= v <= 127 for v in self.scores):
            bad.append("score range")
        if any(v < 0 for v in self.scores):
            bad.append("negative score")
        if not self.roster <= set(range(SEATS)):
            bad.append("roster")
        if not self.luigis <= self.roster:
            bad.append("luigis outside the roster")
        if len(self.stars) > 255:
            bad.append("stars")
        if len(self.events) > RETAIN_EVENTS:
            bad.append("events")
        for s in self.seats.values():
            if len(s.hist) > HISTORY_FRAMES or len(s.done) > DONE_PER_SEAT:
                bad.append("seat tables")
            if len(s.pending_ids) > PENDING_PER_SEAT:
                bad.append("pending")
            if s.grabbed_by is not None:
                h = self.seats.get(s.grabbed_by)
                if h is None or h.holding != s.seat or s.grabbed_by == s.seat:
                    bad.append("grab pair")
            if s.holding is not None:
                o = self.seats.get(s.holding)
                if o is None or o.grabbed_by != s.seat:
                    bad.append("hold pair")
        if len(self.pending) > PENDING_PER_SEAT * SEATS:
            bad.append("pending queue")
        if len(self.caps) > 255 or len(self.cap_wait) > PENDING_PER_SEAT * SEATS:
            bad.append("caps")
        for cid, cap in self.caps.items():
            if cap.cid != cid or not 1 <= cid <= 255:
                bad.append("cap id")
            if cap.placed != (cid <= CAP_PLACED_MAX):
                bad.append("cap kind")
            if cap.taken and not cap.placed:
                bad.append("taken knocked-off cap kept")
        return bad

    def stats(self):
        return {"state": self.state, "seats": len(self.seats),
                "roster": len(self.roster), "states": self.n_states,
                "bad_states": self.n_bad_states, "claims": self.n_claims,
                "verdicts": self.n_verdicts, "pings": self.n_pings,
                "tx": self.n_tx, "starved": self.n_starved,
                "violations": self.n_violations}


def _s32(v):
    v = int(v)
    if v > 0x7FFFFFFF or v < -0x80000000:
        v = ((v + 0x80000000) & 0xFFFFFFFF) - 0x80000000
    return v


def _dist3(a, b):
    return math.sqrt((a.x - b.x) ** 2 + (a.y - b.y) ** 2 + (a.z - b.z) ** 2)


def _facing(victim, attacker):
    """AngleDiff(HorzAngle(victim -> attacker), victim yaw) < 0x4000."""
    ang = int(round(math.atan2(attacker.x - victim.x, attacker.z - victim.z)
                    * 32768.0 / math.pi)) & 0xFFFF
    diff = (ang - (victim.yaw & 0xFFFF)) & 0xFFFF
    if diff >= 0x8000:
        diff = 0x10000 - diff
    return diff < 0x4000
