#!/usr/bin/env python3
"""编程练习1：样例 + 随机对拍 + 极限数据。用法: python3 test.py"""
import random, subprocess, time, os, re, itertools

HERE = os.path.dirname(os.path.abspath(__file__))
MD = os.path.join(os.path.dirname(HERE), "题目.md")
DIG = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


def run(exe, inp, timeout=30):
    t = time.time()
    p = subprocess.run([os.path.join(HERE, exe)], input=inp.encode(), stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=timeout)
    return p.stdout.decode(), time.time() - t, p.returncode


def samples():
    txt = open(MD, encoding='utf-8').read()
    parts = re.split(r'^# T\d+', txt, flags=re.M)[1:]
    res = []
    for p in parts:
        ins = re.findall(r'### 输入 #\d+\s*```\n(.*?)```', p, re.S)
        outs = re.findall(r'### 输出 #\d+\s*```\n(.*?)```', p, re.S)
        res.append(list(zip(ins, outs)))
    return res


# ---- 暴力 ----
def brute1(n, C, items):
    best = 0
    for mask in range(1 << n):
        w = v = 0
        for i in range(n):
            if mask >> i & 1: w += items[i][0]; v += items[i][1]
        if w <= C: best = max(best, v)
    return f"{best}\n"


def brute2(r, m, sN, sT):
    N, T = int(sN, r), int(sT, r)
    # k 按定义暴力找：对 x=0..某范围，m|x 当且仅当 m|(q+kd)
    k = 0
    if m > 1:
        for kk in range(m):
            if all((x % m == 0) == ((x // r + kk * (x % r)) % m == 0) for x in range(0, r * m * 2 + 5)):
                k = kk; break
    seen = set(); x = N; found = False
    while x not in seen:
        seen.add(x)
        if x == T: found = True
        x = x // r + k * (x % r)
    return f"{'Yes' if N % m == 0 else 'No'}\n{k}\n{'Yes' if found else 'No'}\n"


def brute3(a):
    out = []
    s = set()
    for x in a:
        s.add(x)
        last = -10; c = 0
        for y in sorted(s):
            if y - last >= 2: c += 1; last = y
        out.append(c)
    return " ".join(map(str, out)) + "\n"


def tostr(x, r, L=None):
    s = ""
    while x: s = DIG[x % r] + s; x //= r
    s = s or "0"
    if L: s = "0" * random.randint(0, 3) + s
    return s


def check(name, got, exp):
    if got.split() != exp.split():
        print(f"[FAIL] {name}\n got={got[:300]!r}\n exp={exp[:300]!r}"); return False
    return True


def main():
    random.seed(20261019)
    allok = True
    for idx, sm in enumerate(samples()):
        for k, (i, o) in enumerate(sm):
            got, _, _ = run(f"t{idx+1}", i)
            ok = check(f"t{idx+1} sample{k+1}", got, o); allok &= ok
            print(f"t{idx+1} 样例#{k+1}: {'OK' if ok else 'FAIL'}")

    c = 0; T = 500
    for _ in range(T):
        n = random.randint(1, 12); C = random.randint(1, 30)
        items = [(random.randint(1, 35), random.choice([0, random.randint(0, 10 ** 9)])) for _ in range(n)]
        inp = f"{n} {C}\n" + "\n".join(f"{w} {v}" for w, v in items) + "\n"
        got, _, _ = run("t1", inp); ok = check("t1", got, brute1(n, C, items)); allok &= ok; c += ok
        if not ok: open(os.path.join(HERE, "fail1.txt"), "w").write(inp); break
    print(f"t1 随机对拍: {c}/{T}")

    c = 0; T = 1500
    for it in range(T):
        r = random.randint(2, 36)
        m = random.choice([1, random.randint(1, 40), random.randint(1, 300)])
        while __import__('math').gcd(r, m) != 1: m = random.randint(1, 300)
        big = it % 3 == 0
        LN = random.randint(1, 80 if big else 6)
        N = random.randint(0, r ** LN - 1)
        # T: 一半取自真实序列，一半随机
        seq = []; x = N; seen = set()
        kk = pow(r, -1, m) if m > 1 else 0
        while x not in seen: seen.add(x); seq.append(x); x = x // r + kk * (x % r)
        Tv = random.choice(seq) if random.random() < 0.5 else random.randint(0, max(1, N * 2))
        inp = f"{r} {m}\n{tostr(N, r, 1)}\n{tostr(Tv, r, 1)}\n"
        got, _, _ = run("t2", inp); ok = check("t2", got, brute2(r, m, inp.split()[2], inp.split()[3]))
        allok &= ok; c += ok
        if not ok: open(os.path.join(HERE, "fail2.txt"), "w").write(inp); break
    print(f"t2 随机对拍: {c}/{T}")

    c = 0; T = 500
    for _ in range(T):
        n = random.randint(1, 60); hi = random.choice([5, 20, 100])
        a = [random.randint(1, hi) for _ in range(n)]
        inp = f"{n}\n" + " ".join(map(str, a)) + "\n"
        got, _, _ = run("t3", inp); ok = check("t3", got, brute3(a)); allok &= ok; c += ok
        if not ok: open(os.path.join(HERE, "fail3.txt"), "w").write(inp); break
    print(f"t3 随机对拍: {c}/{T}")

    print("---- 极限数据 ----")
    items = [(random.randint(1, 1000), 10 ** 9) for _ in range(1000)]
    inp = "1000 1000\n" + "\n".join(f"{w} {v}" for w, v in items) + "\n"
    out, t, rc = run("t1", inp); print(f"t1 n=C=1000: {t:.3f}s rc={rc} ans={out.strip()}")
    for r, m in [(36, 19997), (2, 19999), (10, 19999), (35, 19994)]:
        import math
        if math.gcd(r, m) != 1: continue
        sN = "".join(random.choice(DIG[:r]) for _ in range(3000)); sN = "1" + sN[1:]
        sT = "".join(random.choice(DIG[:r]) for _ in range(3000))
        inp = f"{r} {m}\n{sN}\n{sT}\n"
        t0 = time.time(); exp = brute2(r, m, sN, sT); tb = time.time() - t0
        out, t, rc = run("t2", inp); ok = check(f"t2 big r={r}", out, exp); allok &= ok
        print(f"t2 L=3000 r={r} m={m}: {t:.3f}s rc={rc} 与暴力一致={ok}")
    a = [random.randint(1, 500000) for _ in range(100000)]
    out, t, rc = run("t3", f"100000\n{' '.join(map(str, a))}\n")
    print(f"t3 n=1e5 随机: {t:.3f}s rc={rc}")
    a = list(range(1, 100001)); random.shuffle(a)
    inp = f"100000\n{' '.join(map(str, a))}\n"
    out, t, rc = run("t3", inp); ok = out.split()[-1] == "50000"; allok &= ok
    print(f"t3 n=1e5 全排列: {t:.3f}s rc={rc} 末项=50000? {ok}")
    print("全部正确" if allok else "存在错误")


if __name__ == "__main__":
    main()
