#!/usr/bin/env python3
"""对拍 + 样例 + 极限数据测试。用法: python3 test.py"""
import random, subprocess, sys, time, os, re

HERE = os.path.dirname(os.path.abspath(__file__))
MD = os.path.dirname(HERE)


def run(exe, inp, timeout=20):
    t = time.time()
    p = subprocess.run([os.path.join(HERE, exe)], input=inp.encode(), stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=timeout)
    return p.stdout.decode(), time.time() - t, p.returncode


def samples(md):
    txt = open(os.path.join(MD, md), encoding='utf-8').read()
    ins = re.findall(r'### 输入 #\d+\s*```\n(.*?)```', txt, re.S)
    outs = re.findall(r'### 输出 #\d+\s*```\n(.*?)```', txt, re.S)
    return list(zip(ins, outs))


# ---------------- T1 ----------------
def gen1(n, q, chain=False):
    par = [0, 0] + [(i - 1 if chain else random.randint(1, i - 1)) for i in range(2, n + 1)]
    adj = [[] for _ in range(n + 1)]
    for i in range(2, n + 1):
        adj[i].append(par[i]); adj[par[i]].append(i)
    V = 10 ** 9
    val = [0] + [random.randint(-V, V) for _ in range(n)]
    lines = [f"{n} {q}"]
    for i in range(1, n + 1):
        nb = adj[i][:]; random.shuffle(nb)
        lines.append(" ".join(map(str, [val[i]] + nb)))
    ev = []
    for _ in range(q):
        t = random.randint(1, 3); u = random.randint(1, n)
        ev.append(f"1 {u} {random.randint(-V, V)}" if t == 1 else f"{t} {u}")
    return "\n".join(lines + ev) + "\n", par, val, ev


def brute1(n, par, val, ev):
    val = val[:]
    ch = [[] for _ in range(n + 1)]
    for i in range(2, n + 1): ch[par[i]].append(i)
    out = []
    for e in ev:
        a = list(map(int, e.split()))
        if a[0] == 1: val[a[1]] = a[2]; continue
        st = [a[1]]; vs = []
        while st:
            u = st.pop(); vs.append(val[u]); st.extend(ch[u])
        out.append(sum(vs) if a[0] == 2 else max(vs))
    return "\n".join(map(str, out)) + ("\n" if out else "")


# ---------------- T2 ----------------
def gen2(a, nl, light_vals=False):
    N = a ** 3
    perm = list(range(1, N + 1)); random.shuffle(perm)  # 坐标 -> 编号
    coords = [(x, y, z) for x in range(a) for y in range(a) for z in range(a)]
    lab = {c: perm[i] for i, c in enumerate(coords)}
    lights = set(random.sample(coords, nl))
    g = {c: (0 if c in lights else random.randint(1, 10 ** 6 - 1)) for c in coords}
    rows = [None] * (N + 1)
    for c in coords:
        x, y, z = c
        nb = []
        for d in [(1,0,0),(-1,0,0),(0,1,0),(0,-1,0),(0,0,1),(0,0,-1)]:
            cc = (x + d[0], y + d[1], z + d[2])
            if cc in lab: nb.append(lab[cc])
        random.shuffle(nb)
        rows[lab[c]] = " ".join(map(str, [g[c]] + nb))
    inp = f"{a}\n" + "\n".join(rows[1:]) + "\n"
    return inp, coords, g, lights


def brute2(a, coords, g, lights):
    import itertools
    L = list(lights)
    D = [(1,0,0),(-1,0,0),(0,1,0),(0,-1,0),(0,0,1),(0,0,-1)]
    rays = []
    for c in L:
        rr = []
        for d in D:
            s = set(); x, y, z = c
            while True:
                x += d[0]; y += d[1]; z += d[2]
                if not (0 <= x < a and 0 <= y < a and 0 <= z < a): break
                s.add((x, y, z))
            rr.append(s)
        rays.append(rr)
    lo, hi = None, None
    for combo in itertools.product(range(6), repeat=len(L)):
        s = set()
        for i, d in enumerate(combo): s |= rays[i][d]
        v = sum(g[c] for c in s)
        lo = v if lo is None else min(lo, v); hi = v if hi is None else max(hi, v)
    return f"{lo} {hi}\n"


# ---------------- T3 ----------------
def gen3(n, m, q, alpha):
    words = ["".join(random.choice("abcdefghijklmnopqrstuvwxyz") for _ in range(random.randint(1, 10)))
             for _ in range(alpha)]
    words = list(dict.fromkeys(words))
    tg = [random.choice(words) for _ in range(n)]
    extra = words + ["zz" + str(i) for i in range(3)]
    sq = [random.choice(extra) for _ in range(m)]
    qs = []
    for _ in range(q):
        l = random.randint(1, n); r = random.randint(l, n); qs.append((l, r))
    inp = "\n".join([str(n)] + tg + [str(m)] + sq + [str(q)] + [f"{l} {r}" for l, r in qs]) + "\n"
    return inp, tg, sq, qs


def brute3(tg, sq, qs):
    out = []
    m = len(sq)
    for l, r in qs:
        need = set(tg[l - 1:r]); best = -1
        for i in range(m):
            seen = set()
            for j in range(i, m):
                if sq[j] in need: seen.add(sq[j])
                if len(seen) == len(need):
                    if best == -1 or j - i + 1 < best: best = j - i + 1
                    break
        out.append(best)
    return "\n".join(map(str, out)) + "\n"


def check(name, got, exp):
    if got.split() != exp.split():
        print(f"[FAIL] {name}\n got={got[:200]!r}\n exp={exp[:200]!r}")
        return False
    return True


def main():
    random.seed(20261019)
    allok = True
    for exe, md in [("t1", "第一题.md"), ("t2", "第二题.md"), ("t3", "第三题.md")]:
        for k, (i, o) in enumerate(samples(md)):
            got, _, _ = run(exe, i)
            ok = check(f"{exe} sample{k+1}", got, o); allok &= ok
            print(f"{exe} 样例#{k+1}: {'OK' if ok else 'FAIL'}")

    # 对拍
    cnt = 0
    for it in range(500):
        n = random.randint(1, 30); q = random.randint(1, 30)
        inp, par, val, ev = gen1(n, q, chain=random.random() < 0.3)
        got, _, _ = run("t1", inp)
        ok = check("t1 rand", got, brute1(n, par, val, ev)); allok &= ok; cnt += ok
        if not ok: open(os.path.join(HERE, "fail1.txt"), "w").write(inp); break
    print(f"t1 随机对拍: {cnt}/500 通过")

    cnt = 0; T = 300
    for it in range(T):
        a = random.randint(2, 5); nl = random.randint(1, min(4, a ** 3))
        inp, coords, g, lights = gen2(a, nl)
        got, _, _ = run("t2", inp)
        ok = check("t2 rand", got, brute2(a, coords, g, lights)); allok &= ok; cnt += ok
        if not ok: open(os.path.join(HERE, "fail2.txt"), "w").write(inp); break
    print(f"t2 随机对拍: {cnt}/{T} 通过")

    cnt = 0
    for it in range(500):
        n = random.randint(1, 12); m = random.randint(1, 40); q = random.randint(1, 15)
        inp, tg, sq, qs = gen3(n, m, q, random.randint(1, 6))
        got, _, _ = run("t3", inp)
        ok = check("t3 rand", got, brute3(tg, sq, qs)); allok &= ok; cnt += ok
        if not ok: open(os.path.join(HERE, "fail3.txt"), "w").write(inp); break
    print(f"t3 随机对拍: {cnt}/500 通过")

    # 极限数据（只测时间与正常退出）
    print("---- 极限数据 ----")
    for chain in (False, True):
        inp, *_ = gen1(10 ** 5, 10 ** 5, chain=chain)
        out, t, rc = run("t1", inp)
        print(f"t1 n=q=1e5 {'链' if chain else '随机树'}: {t:.3f}s rc={rc} 输出行={len(out.split())}")
    for nl in (8,):
        inp, *_ = gen2(70, nl)
        out, t, rc = run("t2", inp)
        print(f"t2 a=70 n={nl}: {t:.3f}s rc={rc} 输出={out.strip()}")
    inp, *_ = gen3(1000, 10 ** 5, 20, 1000)
    out, t, rc = run("t3", inp)
    print(f"t3 n=1000 m=1e5 q=20: {t:.3f}s rc={rc}")
    inp, *_ = gen3(100, 10 ** 4, 2 * 10 ** 5, 100)
    out, t, rc = run("t3", inp)
    print(f"t3 n=100 m=1e4 q=2e5: {t:.3f}s rc={rc}")
    # t3 最坏：n=100 的所有 5050 个不同区间都被问到
    words = [f"w{chr(97+i//26)}{chr(97+i%26)}" for i in range(100)]
    qs = [(l, r) for l in range(1, 101) for r in range(l, 101)]
    qs = (qs * 40)[:2 * 10 ** 5]
    inp = "\n".join(["100"] + words + ["10000"] + [random.choice(words) for _ in range(10 ** 4)] +
                    [str(len(qs))] + [f"{l} {r}" for l, r in qs]) + "\n"
    out, t, rc = run("t3", inp)
    print(f"t3 最坏(5050 个不同区间全覆盖): {t:.3f}s rc={rc}")
    print("全部正确" if allok else "存在错误")


if __name__ == "__main__":
    main()
