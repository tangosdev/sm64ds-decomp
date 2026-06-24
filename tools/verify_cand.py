"""Shared candidate verifier for harvest fan-out agents.

Usage:  python tools/verify_cand.py <FUNC_NAME> <CANDIDATE_FILE>
Looks up the function's target bytes from progress/wl.jsonl, compiles the
candidate (C, or C++ if it starts with //cpp) and runs the reloc-aware oracle.
Prints MATCH or FALSE.
"""
import sys, json, os

sys.path.insert(0, os.path.join(os.path.dirname(__file__)))
import swarm

def main():
    name = sys.argv[1]
    cand = sys.argv[2]
    wl = sys.argv[3] if len(sys.argv) > 3 else "progress/wl.jsonl"
    rec = None
    with open(wl, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            r = json.loads(line)
            if r.get("name") == name:
                rec = r
                break
    if rec is None:
        print("FALSE  (name not in worklist)")
        return
    src = open(cand, "r", encoding="utf-8").read()
    try:
        ok = swarm.oracle_ok(src, name, bytes.fromhex(rec["target_hex"]))
    except Exception as e:
        print("FALSE  (compile/oracle error: %s)" % e)
        return
    print("MATCH" if ok else "FALSE")

if __name__ == "__main__":
    main()
