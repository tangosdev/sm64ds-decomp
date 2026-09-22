"""Run the traced walk_window quietly and wait for its trace file.

Quiet exactly the way battery.py is quiet: CREATE_NO_WINDOW so no console
appears, SW_SHOWMINNOACTIVE so the game window starts minimized and never takes
the foreground, SM64DS_NO_FOCUS so it cannot steal the keyboard. The window is
minimized, NOT hidden.

The trace writes itself when the frame counter passes SM64DS_FN_TRACE_TO, so
this does not need a clean exit: it waits for the file to stop growing, then
ends the process.
"""
import os, subprocess, sys, time

exe, out, frm, to = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
budget = int(sys.argv[5]) if len(sys.argv) > 5 else 180

if os.path.exists(out):
    os.remove(out)

env = dict(os.environ,
           SM64DS_FN_TRACE=out,
           SM64DS_FN_TRACE_FROM=frm,
           SM64DS_FN_TRACE_TO=to,
           SM64DS_FN_TRACE_MB="256",
           SM64DS_NO_FOCUS="1",
           SM64DS_FAULTS_FATAL="1")

si = subprocess.STARTUPINFO()
si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
si.wShowWindow = 7                      # SW_SHOWMINNOACTIVE
flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)

p = subprocess.Popen([exe], cwd=os.path.dirname(exe), env=env,
                     stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                     creationflags=flags, startupinfo=si)
print(f"capture: pid {p.pid}, waiting up to {budget}s for {out}")

size, stable, t0 = -1, 0, time.time()
while time.time() - t0 < budget:
    if p.poll() is not None:
        print(f"capture: process exited on its own, code {p.returncode}")
        break
    if os.path.exists(out):
        now = os.path.getsize(out)
        if now == size and now > 0:
            stable += 1
            if stable >= 3:
                print(f"capture: trace settled at {now:,} bytes")
                break
        else:
            stable = 0
        size = now
    time.sleep(1)

if p.poll() is None:
    p.terminate()
    try: p.wait(timeout=10)
    except subprocess.TimeoutExpired: p.kill()
    print("capture: process ended")

if not os.path.exists(out):
    print("capture: NO TRACE FILE. The run never reached the frame window.")
    sys.exit(1)
print(f"capture: {out} = {os.path.getsize(out):,} bytes")
