"""One-command test scenarios for the PC port, solo or coop (run coopfix1, lane TESTSPAWN1).

A scenario is a JSON file in scenarios/ that says, per player, where he starts and in what state. This script compiles it
into the plain text file the game's SM64DS_SCENARIO knob reads (port/hal/scenario.cpp), gives every player his own folder
(own exe copy, own settings, own save), starts a local relay for a coop room, sets each game's environment and launches.

  python run_scenario.py --list
  python run_scenario.py klepto_hats --exe <walk_window.exe> --aroot <asset root> --out <folder>             (play)
  python run_scenario.py klepto_hats --exe ... --aroot ... --out ... --mode proof [--lane NAME] [--secs N]
  python run_scenario.py --stop klepto_hats --out <folder>

play:  windows visible, sound on, shown without taking the focus; the script returns at once and the games keep running
       until you close them or run --stop.
proof: silent, muted, minimized, never focused; waits for the run-wide row gate first (one slot for up to two games, both
       slots for three or four), takes a picture of every player a set time after he is placed, stops when every picture is
       written (or at --secs), kills only the processes and the relay it started, releases the gate and prints the
       [scenario] lines, the picture paths and the fatal/fault counts.

Paths are never written into the repo: the exe, the asset root and the output folder come from the arguments (or from
SM64DS_ASSET_ROOT / SCENARIO_OUT in the environment). See README.md for the scenario format.
"""
import argparse, hashlib, json, os, re, shutil, socket, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
SCEN_DIR = os.path.join(HERE, "scenarios")
RELAY_PY = os.path.normpath(os.path.join(HERE, "..", "relay", "relay.py"))
ROWGATE = os.environ.get("SCENARIO_ROWGATE")   # the run-wide row gate script (acquire LANE [n] / release LANE); --rowgate overrides
TANGO_RELAY_PORT = 43777          # Tango's own play relay: never bind near it

CHARS = ["mario", "luigi", "wario", "yoshi"]

# Named save presets. A scenario's "save" block may name one and override any field.
PRESETS = {
    "fresh": {},
    "early": {"star_count": 1, "flags": ["messages_seen"]},
    "basement": {"star_count": 10, "characters": ["luigi", "wario"], "flags": ["messages_seen", "basement_key", "basement_door"]},
    "upstairs": {"star_count": 30, "characters": ["luigi", "wario", "yoshi"],
                 "flags": ["messages_seen", "basement_key", "basement_door", "upstairs_key", "upstairs_door", "moat_drained"]},
    "all_open": {"star_count": 105, "characters": ["luigi", "wario", "yoshi"],
                 "flags": ["messages_seen", "basement_key", "basement_door", "upstairs_key", "upstairs_door", "moat_drained",
                           "rabbits_awake"],
                 "cannons": list(range(0, 15))},
}

ROOM_KEYS = {"collision": "SM64DS_ROOM_COLLISION", "damage": "SM64DS_ROOM_DAMAGE", "interact": "SM64DS_ROOM_INTERACT",
             "tp": "SM64DS_COOP_TP"}


def scen_path(name):
    p = name if name.endswith(".json") and os.path.exists(name) else os.path.join(SCEN_DIR, name + ".json")
    if not os.path.exists(p):
        sys.exit("no scenario %s (try --list)" % name)
    return p


def load(name):
    with open(scen_path(name), encoding="utf-8") as f:
        s = json.load(f)
    s.setdefault("name", os.path.splitext(os.path.basename(scen_path(name)))[0])
    if not s.get("players"):
        sys.exit("scenario %s has no players" % s["name"])
    s.setdefault("mode", "coop" if len(s["players"]) > 1 else "solo")
    return s


def merge_save(base, over):
    """A preset by name plus overrides: lists are unioned, scalars replaced, star dicts merged."""
    out = {}
    if base.get("preset"):
        if base["preset"] not in PRESETS:
            sys.exit("unknown save preset %s (have %s)" % (base["preset"], ", ".join(PRESETS)))
        out = json.loads(json.dumps(PRESETS[base["preset"]]))
    for src in (base, over or {}):
        for k, v in src.items():
            if k == "preset":
                continue
            if isinstance(v, list):
                out[k] = sorted(set(out.get(k, [])) | set(v), key=str)
            elif isinstance(v, dict):
                d = dict(out.get(k, {})); d.update(v); out[k] = d
            else:
                out[k] = v
    return out


def save_lines(sv):
    L = []
    if "star_count" in sv:
        L.append("star_count = %d" % int(sv["star_count"]))
    if sv.get("stars"):
        L.append("stars = " + ",".join("%d:%s" % (int(c), str(b).replace("0x", "")) for c, b in sv["stars"].items()))
    if sv.get("flags"):
        L.append("flags = " + ",".join(sv["flags"]))
    if sv.get("characters"):
        L.append("characters = " + ",".join(sv["characters"]))
    if sv.get("cannons"):
        L.append("cannons = " + ",".join(str(int(c)) for c in sv["cannons"]))
    for k in ("flags1", "flags2"):
        if sv.get(k):
            L.append("%s = %s" % (k, sv[k]))
    return L


def player_lines(p, proof):
    L = []
    for k in ("character", "level", "act", "entrance", "area", "power", "power_ticks", "hat", "cap", "health", "lives",
              "settle", "tries"):
        if p.get(k) is not None:
            L.append("%s = %s" % (k, p[k]))
    if p.get("pos"):
        L.append("pos = %d,%d,%d" % tuple(int(v) for v in p["pos"]))
    if p.get("drop") is not None:          # a position put on the floor below it (a near spot always is, unless near.drop is false)
        L.append("drop = %d" % (1 if p["drop"] else 0))
    if p.get("yaw_deg") is not None:
        L.append("yaw_deg = %s" % p["yaw_deg"])
    if p.get("near"):
        n = p["near"]
        L.append("near = %d:%d:%d:%d:%d" % (int(n["class"]), int(n.get("dist", 200)), int(n.get("dy", 0)), int(n.get("index", 0)),
                                           int(round(float(n.get("angle_deg", 0)) * 65536 / 360)) & 0xffff))
        if n.get("param") is not None:
            L.append("near_param = %s" % n["param"])
        if n.get("drop") is not None:
            L.append("drop = %d" % (1 if n["drop"] else 0))
    return L


def compile_file(s, proof, shot_ms):
    """The game's file: [all] then one [pN] block per player."""
    L = ["; written by port/tools/scenario/run_scenario.py from %s.json" % s["name"], "[all]", "name = %s" % s["name"]]
    sv = merge_save(s.get("save", {}), None)
    L += save_lines(sv)
    if proof:
        L.append("shot_ms = %d" % shot_ms)
        cen = s.get("proof", {}).get("census")
        if cen:
            L.append("census = " + ",".join(str(int(c)) for c in cen))
    for k, p in enumerate(s["players"]):
        L.append("[p%d]" % k)
        L += player_lines(p, proof)
        if p.get("save"):
            L += save_lines(merge_save({}, p["save"]))
    return "\n".join(L) + "\n"


def port_free(port):
    """True when nothing holds the port on loopback, TCP or UDP (the relay may use either). A bind alone can succeed on
    Windows while another program listens on the wildcard address, so a listener is also probed by connecting."""
    probe = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    probe.settimeout(0.3)
    try:
        if probe.connect_ex(("127.0.0.1", port)) == 0:
            return False
    finally:
        probe.close()
    for kind in (socket.SOCK_STREAM, socket.SOCK_DGRAM):
        sk = socket.socket(socket.AF_INET, kind)
        try:
            sk.bind(("127.0.0.1", port))
        except OSError:
            return False
        finally:
            sk.close()
    return True


def relay_port():
    """A free loopback port for this run's own relay: start from one derived from this process id, step by 7 until one binds,
    never within 32 of the ports people play on."""
    port = 44100 + (os.getpid() % 400) * 7
    for _ in range(400):
        if abs(port - TANGO_RELAY_PORT) > 32 and abs(port - 51765) > 32 and port_free(port):
            return port
        port += 7
        if port > 47000:
            port = 44100 + (port % 7)
    sys.exit("no free loopback port for the relay between 44100 and 47000")


def proc_info(pid):
    """(executable path, start time in unix seconds, command line) of a live process, or None."""
    ps = ("$p = Get-CimInstance Win32_Process -Filter 'ProcessId=%d'; if ($p) { '{0}|{1}|{2}' -f $p.ExecutablePath, "
          "([DateTimeOffset]$p.CreationDate).ToUnixTimeSeconds(), $p.CommandLine }" % int(pid))
    r = subprocess.run(["powershell", "-NoProfile", "-NonInteractive", "-Command", ps], capture_output=True, text=True)
    line = r.stdout.strip()
    if r.returncode != 0 or "|" not in line:
        return None
    exe, start, cmd = line.split("|", 2)
    try:
        return exe, int(start), cmd
    except ValueError:
        return None


def same_file(a, b):
    return os.path.normcase(os.path.abspath(a or "")) == os.path.normcase(os.path.abspath(b or ""))


def room_code(name):
    return "TS" + hashlib.md5(("%s:%d:%f" % (name, os.getpid(), time.time())).encode()).hexdigest()[:6].upper()


def base_env():
    return {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}


def launch(args):
    s = load(args.scenario)
    proof = args.mode == "proof"
    coop = s["mode"] == "coop"
    n = len(s["players"])
    aroot = args.aroot or os.environ.get("SM64DS_ASSET_ROOT")
    if not aroot:
        sys.exit("give --aroot (the asset root the game reads its ROM files from) or set SM64DS_ASSET_ROOT")
    if not args.exe or not os.path.exists(args.exe):
        sys.exit("give --exe (a built walk_window.exe)")
    out = args.out or os.environ.get("SCENARIO_OUT")
    if not out:
        sys.exit("give --out (a folder for the players' copies, logs and pictures) or set SCENARIO_OUT")
    W = os.path.join(out, s["name"])
    if os.path.isdir(W):
        old = os.path.join(W, "pids.json")
        if os.path.exists(old):
            sys.exit("%s still has a run (pids.json); run --stop %s --out %s first" % (W, s["name"], out))
        shutil.move(W, W + ".prev%d" % int(time.time()))
    os.makedirs(W)
    shot_ms = int(s.get("proof", {}).get("shot_ms", 2000))
    scen_file = os.path.join(W, "scenario.txt")
    with open(scen_file, "w", encoding="utf-8") as f:
        f.write(compile_file(s, proof, shot_ms))

    gate_held = False
    gate = args.rowgate or ROWGATE
    if proof and not gate:
        print("row gate: none set (SCENARIO_ROWGATE or --rowgate); running without one", flush=True)
    if proof and gate:
        need = 1 if n <= 2 else 2
        lane = args.lane
        print("row gate: acquire %s %d" % (lane, need), flush=True)
        r = subprocess.run([sys.executable, gate, "acquire", lane] + ([str(need)] if need > 1 else []))
        if r.returncode != 0:
            sys.exit("row gate refused (%d)" % r.returncode)
        gate_held = True

    NOWIN = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    pids = {"relay": None, "games": [], "lane": args.lane if proof else None, "gate": gate_held}
    procs, fhs = [], []
    relay = None
    try:
        if coop:
            port = relay_port()
            renv = base_env(); renv.update({"SM64DS_RELAY_PORT": str(port), "SM64DS_RELAY_BIND": "127.0.0.1"})
            rl = open(os.path.join(W, "relay.txt"), "wb"); fhs.append(rl)
            relay = subprocess.Popen([sys.executable, "-u", RELAY_PY], env=renv, stdout=rl, stderr=subprocess.STDOUT,
                                     creationflags=NOWIN)
            pids["relay"] = {"pid": relay.pid, "port": port, "started": int(time.time())}
            code = room_code(s["name"])
            print("relay pid %d on 127.0.0.1:%d, room code %s" % (relay.pid, port, code), flush=True)
            time.sleep(2)
        room = s.get("room", {})
        for k, p in enumerate(s["players"]):
            d = os.path.join(W, "c%d" % k)
            os.makedirs(os.path.join(d, "save", "coop"))
            mine = os.path.join(d, "walk_window.exe")
            shutil.copyfile(args.exe, mine)
            for ext in (".map", ".symtab"):
                if os.path.exists(args.exe[:-4] + ext):
                    shutil.copyfile(args.exe[:-4] + ext, mine[:-4] + ext)
            with open(os.path.join(d, "settings.json"), "w") as f:
                json.dump({}, f)
            env = base_env()
            env.update({"SM64DS_ASSET_ROOT": aroot, "SM64DS_INSTANCE": "scenario_%s_%d" % (s["name"], k),
                        "SM64DS_SCENARIO": "%s#%d" % (scen_file, k), "SM64DS_SKIP_MENU": "1",
                        "SM64DS_PLAYER_NAME": "%s%d" % (str(p.get("character", "mario"))[:6], k + 1)})
            ch = p.get("character", "mario")
            env["SM64DS_CHARACTER"] = str(CHARS.index(ch) if ch in CHARS else int(ch))
            if coop:
                env.update({"SM64DS_ADVENTURE": "1", "SM64DS_COOP": "1", "SM64DS_ROOM_KIND": "coop", "SM64DS_COMMS_CODE": code,
                            "SM64DS_COMMS_RELAY": "127.0.0.1:%d" % port,
                            "SM64DS_COOP_SAVE": os.path.join(d, "save", "coop", "coop.coopsav"),
                            "SM64DS_SCENARIO_SAVE_OK": os.path.join(d, "save", "coop", "coop.coopsav"),
                            "SM64DS_COOP_SAVE_NAME": "scenario", "SM64DS_COOP_LOG": "1", "SM64DS_LEVEL": "1",
                            "SM64DS_VOICE_ALLOWED": "0"})
                for rk, ek in ROOM_KEYS.items():
                    env[ek] = str(int(room.get(rk, 1)))
                if k == 0:
                    env["SM64DS_COOP_HOST"] = "1"
            else:
                env["SM64DS_SAVE_PATH"] = env["SM64DS_SCENARIO_SAVE_OK"] = os.path.join(d, "save", "scenario.sav")
            si = subprocess.STARTUPINFO(); si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            flags = 0
            if proof:
                env.update({"SM64DS_NO_FOCUS": "1", "SM64DS_VOLUME": "0", "SM64DS_MINIMIZED": "1", "SM64DS_NO_DIALOG": "1"})
                si.wShowWindow = 7          # SW_SHOWMINNOACTIVE
                flags = NOWIN | 0x4000      # no console, below normal priority
            else:
                si.wShowWindow = 4          # SW_SHOWNOACTIVATE: visible, the focus stays where it is
            env.update({kk: str(v) for kk, v in s.get("env", {}).items()})
            env.update({kk: str(v) for kk, v in p.get("env", {}).items()})
            env.update(dict(x.split("=", 1) for x in args.env))
            fh = open(os.path.join(d, "out.txt"), "wb"); fhs.append(fh)
            pr = subprocess.Popen([mine], cwd=d, env=env, startupinfo=si, stdout=fh, stderr=subprocess.STDOUT, creationflags=flags)
            procs.append(pr)
            pids["games"].append({"pid": pr.pid, "exe": mine, "started": int(time.time())})
            print("player %d (%s) pid %d  folder %s\n  log: %s" % (k, ch, pr.pid, d, os.path.join(d, "playlog", "play_*.log")),
                  flush=True)
            with open(os.path.join(W, "pids.json"), "w") as f:
                json.dump(pids, f)
            time.sleep(1.5)
        print("exe md5 %s" % hashlib.md5(open(args.exe, "rb").read()).hexdigest(), flush=True)
        if not proof:
            print("play: running. Stop with: python %s --stop %s --out %s" % (os.path.basename(__file__), s["name"], out))
            return 0
        secs = int(args.secs or s.get("proof", {}).get("secs", 150 if coop else 90))
        t0 = time.time()
        while time.time() - t0 < secs and any(p.poll() is None for p in procs):
            if all(pictures_done(W, k) for k in range(n)):
                time.sleep(3)
                break
            time.sleep(2)
        print("proof run ended after %.0f s" % (time.time() - t0), flush=True)
    finally:
        if proof:
            stop_pids(W, quiet=True)
            for fh in fhs:
                try: fh.close()
                except Exception: pass
            if gate_held:
                subprocess.run([sys.executable, gate, "release", args.lane])
    if proof:
        report(W, n, args.pics)
    return 0


def player_log(W, k):
    d = os.path.join(W, "c%d" % k, "playlog")
    logs = sorted(os.path.join(d, x) for x in os.listdir(d) if x.startswith("play_")) if os.path.isdir(d) else []
    return logs[-1] if logs else None


def pictures_done(W, k):
    lg = player_log(W, k)
    if not lg:
        return False
    with open(lg, "rb") as f:
        return b"[scenario] picture " in f.read()


def report(W, n, pics):
    total_bad = 0
    for k in range(n):
        d = os.path.join(W, "c%d" % k)
        lg = player_log(W, k)
        print("== player %d  log %s" % (k, lg))
        txt = b""
        for src in (lg, os.path.join(d, "out.txt")):
            if src and os.path.exists(src):
                with open(src, "rb") as f:
                    txt += f.read()
        bad = len(re.findall(rb"FATAL|FAULT code", txt))
        total_bad += bad
        for line in txt.decode("utf-8", "replace").splitlines():
            if line.startswith("[scenario]") and "boot knob" not in line:
                print("  " + line[:220])
        print("  fatal/fault: %d" % bad)
        for b in sorted(x for x in os.listdir(d) if x.endswith(".bmp") and x.startswith("scenario_")):
            src = os.path.join(d, b)
            print("  picture %s" % src)
            if pics:
                os.makedirs(pics, exist_ok=True)
                dst = os.path.join(pics, os.path.basename(W) + "_p%d.png" % k)
                try:
                    from PIL import Image
                    Image.open(src).save(dst)
                    print("  picture png %s" % dst)
                except Exception as e:   # no Pillow: copy the BMP
                    shutil.copyfile(src, dst[:-4] + ".bmp")
                    print("  picture copy %s (%s)" % (dst[:-4] + ".bmp", e))
    print("fatal/fault total: %d" % total_bad)


def stop_pids(W, quiet=False):
    pf = os.path.join(W, "pids.json")
    if not os.path.exists(pf):
        if not quiet:
            print("nothing recorded in %s" % W)
        return
    with open(pf) as f:
        pids = json.load(f)
    # Kill a recorded pid only while it is still the very process this run started: a game must be this run's own exe copy,
    # the relay a python running relay.py, and either must have started within a minute of when the run recorded it. A pid
    # the system has since handed to something else is left alone.
    todo = [("game", g) for g in pids.get("games", []) if isinstance(g, dict)]
    if isinstance(pids.get("relay"), dict):
        todo.append(("relay", pids["relay"]))
    for kind, rec in todo:
        pid = int(rec["pid"])
        info = proc_info(pid)
        if not info:
            verdict = "already gone"
        else:
            exe, start, cmd = info
            fresh = abs(start - int(rec.get("started", 0))) <= 60
            if kind == "game":
                ours = fresh and same_file(exe, rec["exe"])
            else:
                ours = fresh and os.path.basename(exe).lower().startswith("python") and "relay.py" in cmd.replace("\\", "/")
            if ours:
                r = subprocess.run(["taskkill", "/PID", str(pid), "/T", "/F"], capture_output=True)
                verdict = "killed" if r.returncode == 0 else "kill failed"
            else:
                verdict = "LEFT ALONE: now %s, not this run's %s" % (exe or "?", kind)
        if not quiet or verdict.startswith("LEFT") or verdict == "kill failed":
            print("stop %s pid %d: %s" % (kind, pid, verdict))
    os.remove(pf)


def list_scenarios():
    for fn in sorted(os.listdir(SCEN_DIR)):
        if fn.endswith(".json"):
            with open(os.path.join(SCEN_DIR, fn), encoding="utf-8") as f:
                s = json.load(f)
            who = ", ".join(p.get("character", "mario") for p in s.get("players", []))
            print("%-16s %-5s %-24s %s" % (fn[:-5], s.get("mode", "coop" if len(s.get("players", [])) > 1 else "solo"), who,
                                          s.get("about", "")))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("scenario", nargs="?")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--stop", metavar="NAME")
    ap.add_argument("--mode", choices=["play", "proof"], default="play")
    ap.add_argument("--rowgate", help="row gate script for proof mode (default SCENARIO_ROWGATE)")
    ap.add_argument("--exe")
    ap.add_argument("--aroot")
    ap.add_argument("--out")
    ap.add_argument("--pics", help="proof: also copy each player's picture here as <scenario>_p<k>.png")
    ap.add_argument("--lane", help="proof: the name the row gate ticket carries (default SCENARIO-<pid>, unique per run)")
    ap.add_argument("--secs", type=int)
    ap.add_argument("--env", action="append", default=[], metavar="K=V", help="an extra knob for every game (repeatable)")
    ap.add_argument("--print", action="store_true", help="print the compiled game file and stop")
    a = ap.parse_args()
    if not a.lane:
        a.lane = "SCENARIO-%d" % os.getpid()   # one ticket name per run, so two runs never release each other's slots
    if a.list:
        return list_scenarios()
    if a.stop:
        out = a.out or os.environ.get("SCENARIO_OUT")
        if not out:
            sys.exit("give --out")
        return stop_pids(os.path.join(out, a.stop))
    if not a.scenario:
        ap.print_help(); return 1
    if a.print:
        s = load(a.scenario)
        sys.stdout.write(compile_file(s, a.mode == "proof", int(s.get("proof", {}).get("shot_ms", 2000))))
        return 0
    return launch(a)


if __name__ == "__main__":
    sys.exit(main() or 0)
