"""Verify + record a harvest candidate in one step (used by fan-out agents).

  MATCH path:    python tools/try_cand.py <AGENT_N> <FUNC_NAME> <CAND_FILE>
      Compiles the candidate, runs the reloc-aware oracle. On MATCH, appends
      {"name","module","addr","c_source"} to C:/tmp/h<N>_matches.jsonl and prints MATCH.
      On failure prints FALSE (nothing recorded).

  NEAR-MISS path: python tools/try_cand.py <AGENT_N> <FUNC_NAME> <CAND_FILE> --nearmiss "note"
      Appends {"name","module","addr","c_source","note"} to C:/tmp/h<N>_nearmiss.jsonl.

Looks the function up in progress/wl.jsonl for target bytes / module / addr.
"""
import sys, json, os

sys.path.insert(0, os.path.dirname(__file__))
import swarm

WL = "progress/wl.jsonl"

def find(name):
    with open(WL, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            r = json.loads(line)
            if r.get("name") == name:
                return r
    return None

def append(path, obj):
    with open(path, "a", encoding="utf-8") as f:
        f.write(json.dumps(obj) + "\n")

def main():
    n = sys.argv[1]
    name = sys.argv[2]
    cand = sys.argv[3]
    nearmiss = None
    if "--nearmiss" in sys.argv:
        i = sys.argv.index("--nearmiss")
        nearmiss = sys.argv[i + 1] if i + 1 < len(sys.argv) else ""
    rec = find(name)
    if rec is None:
        print("FALSE  (name not in worklist)")
        return
    src = open(cand, "r", encoding="utf-8").read()
    if nearmiss is not None:
        append("C:/tmp/h%s_nearmiss.jsonl" % n,
               {"name": name, "module": rec["module"], "addr": rec["addr"],
                "c_source": src, "note": nearmiss})
        print("NEARMISS recorded")
        return
    try:
        ok = swarm.oracle_ok(src, name, bytes.fromhex(rec["target_hex"]))
    except Exception as e:
        print("FALSE  (compile/oracle error: %s)" % e)
        return
    if ok:
        append("C:/tmp/h%s_matches.jsonl" % n,
               {"name": name, "module": rec["module"], "addr": rec["addr"],
                "c_source": src})
        print("MATCH")
    else:
        print("FALSE")

if __name__ == "__main__":
    main()
