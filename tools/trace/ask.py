"""ask.py -- pause gameplay and ask a blinded question about what just changed.

Runtime evidence for field names: at a pause the tool pokes a field (or lets
the game write it, at a trigger breakpoint), advances a few frames, captures
before/after screens and asks "what changed?" without saying whether anything
was written. Sham pauses (no write, a write of the same value, an unused bit)
are mixed in at random. The key lives in a separate file; `score` unblinds.

  serve   launch the installed melonDS (swapped config: GDB stub on, JIT off), load a
          savestate, and hold the ONE stub connection for every later command
  run     run an experiment spec (JSON): --mode human | agent | none
  score   unblind a finished run and tally the answers
  probe   arm a spec's probes (log-and-continue breakpoints) for N seconds
  ctl     one daemon op, e.g. `ctl step n=3`, `ctl read addr=0x020ad634 n=12`
  stop    detach cleanly (and --kill melonDS, --clean its work directory)

See README.md ("ask.py: pause and ask") for the spec format and a walkthrough.
"""
import argparse
import json
import os
import random
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import emusession as es  # noqa: E402

DEFAULT_WORKDIR = os.path.join(tempfile.gettempdir(), 'sm64ds-ask-emu')


# ----------------------------------------------------------------------- serve
def cmd_serve(a):
    import emuwin
    images = es.Images(a.extracted)
    os.makedirs(a.logdir, exist_ok=True)
    proc = None
    if a.attach:
        hwnd = emuwin.find_window()
        if not hwnd:
            sys.exit('no melonDS window to attach to')
        pid = emuwin.wt.DWORD()
        emuwin.user32.GetWindowThreadProcessId(hwnd, emuwin.ctypes.byref(pid))
        pid = pid.value
    else:
        if emuwin.find_window():
            sys.exit('a melonDS window is already open: close it, or use --attach')
        rom = a.rom or es.find_input('sm64.nds')
        exe = a.melonds or emuwin.find_melonds()
        emuwin.prepare_workdir(a.workdir, rom, state=a.state, slot=1, melonds=exe, port=a.port, sav=a.sav)
        try:
            proc, hwnd = emuwin.launch(a.workdir, exe)
        except Exception:
            print(json.dumps({'config_restored': emuwin.restore_config(a.workdir)}), flush=True)
            raise
        pid = proc.pid
        time.sleep(a.boot_wait)
        emuwin.fit_window(hwnd, a.scale)
        if a.state:
            emuwin.load_state_slot(pid, 1)
            time.sleep(1.5)
    s = es.Session(images, port=a.port, log_path=os.path.join(a.logdir, 'session.jsonl'))
    stop = s.connect(timeout=90)
    s.cont()
    d = es.Daemon(s, a.rpc, proc=proc, hwnd=hwnd, pid=pid)
    print(json.dumps({'ready': True, 'pid': pid, 'rpc': a.rpc, 'stub': a.port, 'attach_stop': stop,
                      'workdir': None if a.attach else a.workdir}), flush=True)
    try:
        d.serve()
    finally:
        if proc is not None:
            if d.kill_emu:
                emuwin.kill(proc.pid)
            try:
                proc.wait(None if not d.kill_emu else 15)   # melonDS rewrites its toml on exit
            except Exception:
                pass
            time.sleep(0.5)
            rc = emuwin.restore_config(a.workdir)
            print(json.dumps({'config_restored': rc}), flush=True)
            if d.clean and (rc is None or rc[1] == rc[2]):   # never delete an unrestored backup
                import shutil
                for _ in range(10):
                    try:
                        shutil.rmtree(a.workdir)
                        break
                    except OSError:
                        time.sleep(0.5)
    print(json.dumps({'stopped': True}), flush=True)


def cmd_restore_config(a):
    import emuwin
    print(json.dumps({'config_restored': emuwin.restore_config(a.workdir)}))


def spawn_serve(a):
    """Start `serve` detached and wait until it answers."""
    os.makedirs(a.out, exist_ok=True)
    args = [sys.executable, os.path.abspath(__file__), 'serve', '--rpc', str(a.rpc),
            '--logdir', os.path.join(a.out, 'key')]
    if a.state:
        args += ['--state', a.state]
    if a.workdir:
        args += ['--workdir', a.workdir]
    log = open(os.path.join(a.out, 'serve.log'), 'a')
    subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, creationflags=0x00000008 | 0x00000200)
    t0 = time.time()
    while time.time() - t0 < 150:
        if es.daemon_up(a.rpc):
            return
        time.sleep(1)
    sys.exit('serve did not come up; see %s' % os.path.join(a.out, 'serve.log'))


# ------------------------------------------------------------------------ run
def load_spec(path):
    with open(path, encoding='utf-8') as f:
        return json.load(f)


def build_plan(spec, rng):
    plan = []
    for c in spec['conditions']:
        for _ in range(int(c.get('count', 1))):
            plan.append(c['name'])
    rng.shuffle(plan)
    return plan


def expected_answer(cond, before):
    exp = cond.get('expect')
    if isinstance(exp, dict):
        v = before.get(exp['field'])
        if not isinstance(v, int):
            return None
        return exp['if_set'] if v & es.parse_int(exp['mask']) else exp['if_clear']
    return exp


def setup_bps(c, spec):
    c.clear_bps()
    fr = spec['frame']
    c.set_frame(addr=fr['addr'], module=fr.get('module'), name=fr.get('name', 'frame'))
    for t in spec.get('triggers', []):
        c.add_bp(addr=t['addr'], role='trigger', name=t['name'], module=t.get('module'),
                 obj=t.get('obj'), when=t.get('when'), dump=t.get('dump', []), arm=False)
    for p in spec.get('probes', []):
        c.add_bp(addr=p['addr'], role='probe', name=p['name'], module=p.get('module'), obj=p.get('obj'),
                 when=p.get('when'), dump=p.get('dump', []), ret=p.get('ret', False),
                 max_hits=p.get('max_hits', 0), arm=False)


def check_residency(c, spec):
    out = {}
    for r in spec.get('residency', []):
        out[r['module']] = c.resident(module=r['module'], addr=r.get('addr'))
    return out


def ask_popup(qid, pair_png, question, choices, confidences, answer_after=None, shot_path=None):
    """Topmost Tk window: the pair, the question, choice + confidence. Shows only
    the question id; whether the pause was real or a sham is never shown."""
    import tkinter as tk
    root = tk.Tk()
    root.title('ask.py  %s' % qid)
    root.attributes('-topmost', True)
    img = tk.PhotoImage(file=pair_png)
    hdr = tk.Frame(root)
    hdr.pack(fill='x')
    tk.Label(hdr, text='A: before', font=('Segoe UI', 11, 'bold')).pack(side='left', padx=10)
    tk.Label(hdr, text='B: after', font=('Segoe UI', 11, 'bold')).pack(side='right', padx=10)
    tk.Label(root, image=img).pack()
    tk.Label(root, text=question, wraplength=img.width(), justify='left',
             font=('Segoe UI', 11)).pack(fill='x', padx=8, pady=6)
    choice = tk.StringVar(value='')
    conf = tk.StringVar(value='')
    row = tk.Frame(root)
    row.pack(pady=2)
    for ch in choices:
        tk.Radiobutton(row, text=ch, variable=choice, value=ch, indicatoron=0, width=18,
                       font=('Segoe UI', 10)).pack(side='left', padx=3)
    row2 = tk.Frame(root)
    row2.pack(pady=2)
    tk.Label(row2, text='confidence:').pack(side='left')
    for cf in confidences:
        tk.Radiobutton(row2, text=cf, variable=conf, value=cf, indicatoron=0, width=8).pack(side='left', padx=2)
    result = {}

    def submit():
        if not choice.get():
            return
        result.update(answer=choice.get(), confidence=conf.get() or None, t=round(time.time(), 3))
        root.destroy()
    btn = tk.Button(root, text='Submit', command=submit, width=14)
    btn.pack(pady=6)
    root.protocol('WM_DELETE_WINDOW', lambda: None)   # must answer
    if answer_after is not None:
        def auto():
            choice.set(choices[0])
            conf.set(confidences[-1])
            root.update()
            if shot_path:
                import emuwin
                h = emuwin.user32.GetAncestor(root.winfo_id(), 2)
                emuwin.capture_client(h).save_png(shot_path)
            result['auto'] = True
            submit()
        root.after(int(answer_after), auto)
    root.mainloop()
    return result


def cmd_run(a):
    spec = load_spec(a.spec)
    if not es.daemon_up(a.rpc):
        if not a.launch:
            sys.exit('no daemon on port %d: start `ask.py serve` or pass --launch' % a.rpc)
        spawn_serve(a)
    c = es.Client(a.rpc)
    st = c.status()
    if st['dead']:
        sys.exit('stub connection is down: %s' % st['dead'])
    out = a.out
    pairs = os.path.join(out, 'pairs')
    keyd = os.path.join(out, 'key')
    for d in (pairs, keyd, os.path.join(keyd, 'frames')):
        os.makedirs(d, exist_ok=True)
    seed = a.seed if a.seed is not None else int.from_bytes(os.urandom(6), 'little')
    rng = random.Random(seed)
    conds = {k['name']: k for k in spec['conditions']}
    plan = build_plan(spec, rng)
    q = spec['question']
    choices = q['choices']
    confidences = q.get('confidence', ['high', 'medium', 'low'])
    cap = spec.get('capture', {})
    which = cap.get('screen', 'top')
    after_frames = int(spec.get('after_frames', 4))
    lo, hi = spec.get('pause', {}).get('countdown', [10, 40])
    fields = spec.get('fields', [])
    logf = open(os.path.join(keyd, 'log.jsonl'), 'a', encoding='utf-8')
    ev0 = st['nevents']

    if spec.get('load_state_slot') and not a.no_reload:
        c.load_state(slot=spec['load_state_slot'])
    setup_bps(c, spec)
    res = check_residency(c, spec)
    logf.write(json.dumps({'ev': 'start', 'spec': os.path.abspath(a.spec), 'seed': seed, 'residency': res,
                           't': time.time()}) + '\n')
    if not all(res.values()):
        sys.exit('residency check failed: %s (wrong scene for this spec?)' % res)
    if spec.get('settle_frames'):
        c.step(n=int(spec['settle_frames']))

    key = {'spec': os.path.abspath(a.spec), 'name': spec.get('name'), 'seed': seed, 'mode': a.mode,
           'question': q['text'], 'choices': choices, 'events': {}}
    answers = {'spec': spec.get('name'), 'question': q['text'], 'choices': choices,
               'confidence_choices': confidences,
               'how': 'pairs/<id>.png: left = A (before), right = B (after). Fill answer (one of choices) '
                      'and confidence for each id. Do not open key/ before scoring.',
               'answers': {}}
    print(json.dumps({'run': out, 'events': len(plan), 'mode': a.mode}), flush=True)
    for i, cname in enumerate(plan):
        qid = 'Q%02d' % (i + 1)
        cond = conds[cname]
        rec = {'ev': 'event', 'id': qid, 'condition': cname, 'real': bool(cond.get('real'))}
        env = {}
        try:
            if spec.get('aim') and spec.get('locate'):
                # same for every condition, real or sham: e.g. put the camera on the subject,
                # then the countdown lets the camera settle before the pause
                c.step(n=1)
                o = c.locate(locate=spec['locate'])
                if o is not None:
                    rec['aim'] = [c.poke(expr=p['expr'], op=p.get('op', 'write'), mask=p.get('mask', 0xffffffff),
                                         value=p.get('value', 0), size=p.get('size', 4), env={'obj': o})
                                  for p in spec['aim']]
            if cond.get('trigger'):
                c.arm(addr=[t for t in spec['triggers'] if t['name'] == cond['trigger']][0]['addr'], on=True)
                hit = c.wait_trigger(names=[cond['trigger']], timeout=float(cond.get('timeout', 60)))
                c.arm(addr=[t for t in spec['triggers'] if t['name'] == cond['trigger']][0]['addr'], on=False)
                if hit is None:
                    rec['status'] = 'no_trigger'
                    logf.write(json.dumps(rec) + '\n')
                    print(json.dumps({'id': qid, 'status': 'skipped'}), flush=True)
                    continue
                rec['pause'] = {'trigger': hit.get('name'), 'pc': hit.get('pc'), 'lr': hit['regs']['lr']}
                obj = int(hit['obj'], 16) if hit.get('obj') else None
            else:
                n = rng.randint(int(lo), int(hi))
                hit = c.step(n=n)
                rec['pause'] = {'countdown': n, 'frames': hit.get('frames')}
                obj = None
            if obj is None and spec.get('locate'):
                obj = c.locate(locate=spec['locate'])
            if obj is not None:
                env['obj'] = obj
                rec['obj'] = '%08x' % obj
            elif cond.get('pokes') or cond.get('prepare'):
                rec['status'] = 'object_not_found'
                logf.write(json.dumps(rec) + '\n')
                c.go()
                print(json.dumps({'id': qid, 'status': 'skipped'}), flush=True)
                continue

            def do_pokes(lst):
                return [c.poke(expr=p['expr'], op=p.get('op', 'set'), mask=p.get('mask', 0xff),
                               value=p.get('value', 0), size=p.get('size', 1), env=env) for p in lst]
            # prepare: put the object into the state the question starts from (shown in A)
            rec['prepare'] = do_pokes(cond.get('prepare', []))
            if rec['prepare']:
                c.step(n=int(cond.get('prepare_frames', after_frames)))
            rec['before'] = c.snapshot(fields=fields, env=env)
            bpng = os.path.join(keyd, 'frames', qid + '_A.png')
            rec['shot_before'] = c.shot(path=bpng, which=which, settle_ms=cap.get('settle_ms', 250))
            rec['pokes'] = do_pokes(cond.get('pokes', []))
            c.step(n=after_frames)
            rec['after'] = c.snapshot(fields=fields, env=env)
            apng = os.path.join(keyd, 'frames', qid + '_B.png')
            rec['shot_after'] = c.shot(path=apng, which=which, settle_ms=cap.get('settle_ms', 250))
            if cond.get('restore', True):
                # put every written byte back to its value before this event, newest first
                for p in reversed(rec['prepare'] + rec['pokes']):
                    c.poke(expr='0x' + p['addr'], op='write', mask=(1 << (8 * p['size'])) - 1,
                           value=p['old'], size=p['size'])
                rec['restored'] = True
            rec['expected'] = expected_answer(cond, rec['before'])
            pair = os.path.join(pairs, qid + '.png')
            compose_pair(bpng, apng, pair)
            rec['pair'] = pair
            rec['status'] = 'ok'
            key['events'][qid] = {k: rec[k] for k in ('condition', 'real', 'expected', 'obj', 'prepare', 'pokes',
                                                      'pause', 'before', 'after') if k in rec}
            if a.mode == 'human':
                ans = ask_popup(qid, pair, q['text'], choices, confidences, a.answer_after,
                                os.path.join(out, 'popup_%s.png' % qid) if a.answer_after is not None else None)
                answers['answers'][qid] = ans
            elif a.mode == 'agent':
                answers['answers'][qid] = {'answer': None, 'confidence': None}
            logf.write(json.dumps(rec) + '\n')
            logf.flush()
            c.go()
            print(json.dumps({'id': qid, 'status': 'ok'}), flush=True)
        except es.RemoteError as e:
            rec['status'] = 'error'
            rec['error'] = str(e)
            logf.write(json.dumps(rec) + '\n')
            print(json.dumps({'id': qid, 'status': 'error', 'error': str(e)}), flush=True)
            if 'stub_dropped' in str(e):
                break
            try:
                c.go()
            except es.RemoteError:
                pass
    evs = c.events(since=ev0, max=100000)
    with open(os.path.join(keyd, 'events.jsonl'), 'w', encoding='utf-8') as f:
        for e in evs:
            f.write(json.dumps(e) + '\n')
    for t in spec.get('triggers', []) + spec.get('probes', []):
        try:
            c.arm(addr=t['addr'], on=False)
        except es.RemoteError:
            pass
    with open(os.path.join(keyd, 'key.json'), 'w', encoding='utf-8') as f:
        json.dump(key, f, indent=1)
    if a.mode != 'none':
        with open(os.path.join(out, 'answers.json'), 'w', encoding='utf-8') as f:
            json.dump(answers, f, indent=1)
    print(json.dumps({'done': True, 'answers': os.path.join(out, 'answers.json') if a.mode != 'none' else None,
                      'key_dir': keyd}), flush=True)


def read_png(path):
    """Decode the 8-bit RGB, non-interlaced PNGs emuwin writes."""
    import struct
    import zlib
    data = open(path, 'rb').read()
    pos, idat, w, h = 8, b'', 0, 0
    while pos < len(data):
        n, tag = struct.unpack('>I4s', data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + n]
        if tag == b'IHDR':
            w, h = struct.unpack('>II', body[:8])
        elif tag == b'IDAT':
            idat += body
        pos += 12 + n
    raw = zlib.decompress(idat)
    stride = w * 3
    out = bytearray()
    for r in range(h):
        ft = raw[r * (stride + 1)]
        line = bytearray(raw[r * (stride + 1) + 1:(r + 1) * (stride + 1)])
        if ft != 0:
            raise ValueError('filtered PNG rows are not supported (%s)' % path)
        out += line
    import emuwin
    return emuwin.Frame(w, h, out)


def compose_pair(a_png, b_png, out_png):
    import emuwin
    emuwin.Frame.hstack([read_png(a_png), read_png(b_png)], gap=8).save_png(out_png)


# ---------------------------------------------------------------------- score
def binom_tail(k, n, p):
    from math import comb
    return sum(comb(n, i) * p ** i * (1 - p) ** (n - i) for i in range(k, n + 1))


def cmd_score(a):
    out = a.run
    key = json.load(open(os.path.join(out, 'key', 'key.json'), encoding='utf-8'))
    ans = json.load(open(a.answers or os.path.join(out, 'answers.json'), encoding='utf-8'))
    abstain = set(x for x in key['choices'] if 'tell' in x.lower() or 'unsure' in x.lower())
    rows, per = [], {}
    k = n = 0
    for qid in sorted(key['events']):
        ev = key['events'][qid]
        a1 = ans['answers'].get(qid) or {}
        got, conf = a1.get('answer'), a1.get('confidence')
        if a1.get('auto'):  # --answer-after smoke test: not a judgement
            status = 'auto'
        else:
            status = 'abstain' if (got is None or got in abstain) else ('hit' if got == ev['expected'] else 'miss')
        if status in ('hit', 'miss'):
            n += 1
            k += status == 'hit'
        p = per.setdefault(ev['condition'], {'real': ev['real'], 'hit': 0, 'miss': 0, 'abstain': 0, 'auto': 0})
        p[status] += 1
        rows.append({'id': qid, 'condition': ev['condition'], 'real': ev['real'], 'expected': ev['expected'],
                     'answer': got, 'confidence': conf, 'result': status})
    nchoice = max(1, len([c for c in key['choices'] if c not in abstain]))
    summary = {'answered': n, 'correct': k, 'chance_per_item': round(1.0 / nchoice, 3),
               'p_value_at_least_k_by_chance': binom_tail(k, n, 1.0 / nchoice) if n else None,
               'by_condition': per, 'rows': rows}
    with open(os.path.join(out, 'score.json'), 'w', encoding='utf-8') as f:
        json.dump(summary, f, indent=1)
    for r in rows:
        print('%-4s %-14s real=%-5s expected=%-18s answer=%-18s %-7s %s' % (
            r['id'], r['condition'], r['real'], r['expected'], r['answer'], r['confidence'], r['result']))
    print(json.dumps({k2: v for k2, v in summary.items() if k2 != 'rows'}, indent=1))


# ---------------------------------------------------------------------- probe
def cmd_probe(a):
    spec = load_spec(a.spec)
    c = es.Client(a.rpc)
    os.makedirs(a.out, exist_ok=True)
    st = c.status()
    setup_bps(c, spec)
    res = check_residency(c, spec)
    c.step(n=1)      # arm everything while halted (see Daemon._halted)
    for p in spec.get('probes', []):
        c.arm(addr=p['addr'], on=True)
    aim = None
    if a.aim and spec.get('aim') and spec.get('locate'):
        # one setup action with the probes already armed (e.g. a stomp), so nothing is missed
        c.step(n=1)
        o = c.locate(locate=spec['locate'])
        if o is None:
            sys.exit('locate found nothing; aim not applied')
        aim = [c.poke(expr=p['expr'], op=p.get('op', 'write'), mask=p.get('mask', 0xffffffff),
                      value=p.get('value', 0), size=p.get('size', 4), env={'obj': o}) for p in spec['aim']]
    c.run_for(seconds=a.seconds)
    c.step(n=1)
    for p in spec.get('probes', []):
        c.arm(addr=p['addr'], on=False)
    c.go()
    evs = c.events(since=st['nevents'], max=100000)
    path = os.path.join(a.out, 'probe_%s.jsonl' % time.strftime('%H%M%S'))
    with open(path, 'w', encoding='utf-8') as f:
        f.write(json.dumps({'ev': 'probe_start', 'spec': os.path.abspath(a.spec), 'residency': res,
                            'aim': aim}) + '\n')
        for e in evs:
            f.write(json.dumps(e) + '\n')
    counts = {}
    for e in evs:
        counts[(e['ev'], e.get('name'))] = counts.get((e['ev'], e.get('name')), 0) + 1
    print(json.dumps({'log': path, 'residency': res, 'counts': {'%s:%s' % k: v for k, v in counts.items()}}))


# ------------------------------------------------------------------ ctl, stop
def cmd_ctl(a):
    c = es.Client(a.rpc)
    kw = {}
    for item in a.args:
        k, v = item.split('=', 1)
        try:
            kw[k] = json.loads(v)
        except ValueError:
            kw[k] = v
    print(json.dumps(c.call(a.op, **kw), indent=1))


def cmd_stop(a):
    if not es.daemon_up(a.rpc):
        print('no daemon')
        return
    es.Client(a.rpc).quit(kill_emu=a.kill, clean=a.clean)
    t0 = time.time()
    while es.daemon_up(a.rpc) and time.time() - t0 < 20:
        time.sleep(0.3)
    print('stopped')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--rpc', type=int, default=es.DEFAULT_RPC_PORT, help='daemon port (default 3390)')
    sub = ap.add_subparsers(dest='cmd', required=True)

    s = sub.add_parser('serve', help='launch/attach melonDS and hold the stub connection')
    s.add_argument('--state', help='melonDS savestate file to load (copied into the work dir as slot 1)')
    s.add_argument('--rom', help='ROM (default: sm64.nds of this or the main checkout)')
    s.add_argument('--sav', help='optional .sav to copy next to the ROM copy')
    s.add_argument('--workdir', default=DEFAULT_WORKDIR, help='private melonDS copy (outside the repo)')
    s.add_argument('--melonds', help='melonDS.exe to run (default: found on PATH / winget install)')
    s.add_argument('--port', type=int, default=3333, help='ARM9 GDB stub port')
    s.add_argument('--attach', action='store_true', help='attach to an already running melonDS instead')
    s.add_argument('--extracted', help='extracted/dsd (default: this or the main checkout)')
    s.add_argument('--logdir', default=os.path.join(DEFAULT_WORKDIR, 'logs'))
    s.add_argument('--scale', type=int, default=2, help='window scale for captures')
    s.add_argument('--boot-wait', type=float, default=3.0)
    s.set_defaults(fn=cmd_serve)

    r = sub.add_parser('run', help='run an experiment spec')
    r.add_argument('spec')
    r.add_argument('--mode', choices=['human', 'agent', 'none'], default='agent')
    r.add_argument('--out', required=True, help='run directory (outside the repo)')
    r.add_argument('--seed', type=int)
    r.add_argument('--launch', action='store_true', help='start `serve` if no daemon is up')
    r.add_argument('--state', help='with --launch: savestate to load')
    r.add_argument('--workdir', help='with --launch: melonDS work dir')
    r.add_argument('--no-reload', action='store_true', help="skip the spec's load_state_slot")
    r.add_argument('--answer-after', type=int, help='human mode smoke test: auto-answer after N ms')
    r.set_defaults(fn=cmd_run)

    sc = sub.add_parser('score', help='unblind a run and tally answers')
    sc.add_argument('run')
    sc.add_argument('--answers', help='answers file (default RUN/answers.json)')
    sc.set_defaults(fn=cmd_score)

    p = sub.add_parser('probe', help="arm a spec's probes for N seconds and save the log")
    p.add_argument('spec')
    p.add_argument('--seconds', type=float, default=10)
    p.add_argument('--out', required=True)
    p.add_argument('--aim', action='store_true', help="apply the spec's aim pokes once, after arming")
    p.set_defaults(fn=cmd_probe)

    c = sub.add_parser('ctl', help='one daemon op: ctl <op> key=value ...')
    c.add_argument('op')
    c.add_argument('args', nargs='*')
    c.set_defaults(fn=cmd_ctl)

    t = sub.add_parser('stop', help='detach the daemon')
    t.add_argument('--kill', action='store_true',
                   help="also close melonDS (the user's melonDS.toml is restored once melonDS has exited)")
    t.add_argument('--clean', action='store_true', help='with --kill: delete the work dir (ROM copy, states)')
    t.set_defaults(fn=cmd_stop)

    rc = sub.add_parser('restore-config', help="put the user's melonDS.toml back (after a crash)")
    rc.add_argument('--workdir', default=DEFAULT_WORKDIR)
    rc.set_defaults(fn=cmd_restore_config)

    a = ap.parse_args()
    a.fn(a)


if __name__ == '__main__':
    main()
