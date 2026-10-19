// T836206 2-2 陈光之序
// 思路：
// 1) 编号与空间位置无关，只给出邻接关系，需先还原坐标。网格图的最短路 = 曼哈顿距离。
//    度为 3 的点是 8 个角。取一个角 O，与 O 距离为 a-1 的三个角记为 X、Y、Z（分别是三条棱的另一端）。
//    对任意格子 p=(x,y,z)：d(O,p)=x+y+z，d(X,p)=(a-1-x)+y+z，故 x=(d(O,p)-d(X,p)+a-1)/2，y、z 同理。
//    做 4 次 BFS 即可，O(a^3)。（a=2 时 8 个点度都为 3，同样成立。）
// 2) n<=8 个光源，每个 6 个方向，共 6^8≈1.68e6 种组合。DFS 枚举，用计数数组增量维护被照亮格子：
//    计数 0->1 时加上观赏值，回溯时 1->0 减去。每束光至多 a-1<=69 格，总运算量约 3e8 次简单操作。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char *buf;
static size_t len, pos_ = 0;
bool nextLine(char *&b, char *&e) {
    while (pos_ < len) {
        size_t s = pos_;
        while (pos_ < len && buf[pos_] != '\n') pos_++;
        size_t t = pos_;
        if (pos_ < len) pos_++;
        bool nonblank = false;
        for (size_t i = s; i < t; i++)
            if (!isspace((unsigned char)buf[i])) { nonblank = true; break; }
        if (nonblank) { b = buf + s; e = buf + t; return true; }
    }
    return false;
}
bool readInt(char *&p, char *e, ll &x) {
    while (p < e && !(*p >= '0' && *p <= '9')) p++;
    if (p >= e) return false;
    x = 0;
    while (p < e && *p >= '0' && *p <= '9') x = x * 10 + (*p++ - '0');
    return true;
}

int N, A;
vector<int> nb, deg;  // nb[u*6+k]：u 的第 k 个邻居
vector<int> bfs(int s) {
    vector<int> d(N + 1, -1);
    vector<int> qu;
    qu.reserve(N);
    qu.push_back(s);
    d[s] = 0;
    for (size_t h = 0; h < qu.size(); h++) {
        int u = qu[h];
        const int *p = &nb[(size_t)u * 6];
        for (int k = 0; k < deg[u]; k++) {
            int w = p[k];
            if (d[w] < 0) { d[w] = d[u] + 1; qu.push_back(w); }
        }
    }
    return d;
}

vector<ll> g;
vector<int> cntLit;
vector<array<vector<int>, 6>> rays;
int L;
ll cur = 0, best_min = LLONG_MAX, best_max = LLONG_MIN;

void dfs(int k) {
    if (k == L) {  // 仅当 L==0 时到达（实际 n>=1）
        best_min = min(best_min, cur);
        best_max = max(best_max, cur);
        return;
    }
    if (k == L - 1) {  // 最后一个光源：只需累加未被照亮格子的增量，无需标记/回溯
        for (int d = 0; d < 6; d++) {
            ll add = 0;
            for (int c : rays[k][d])
                if (cntLit[c] == 0) add += g[c];
            best_min = min(best_min, cur + add);
            best_max = max(best_max, cur + add);
        }
        return;
    }
    for (int d = 0; d < 6; d++) {
        const vector<int> &r = rays[k][d];
        for (int c : r)
            if (cntLit[c]++ == 0) cur += g[c];
        dfs(k + 1);
        for (int c : r)
            if (--cntLit[c] == 0) cur -= g[c];
    }
}

int main() {
    {  // 一次性读入全部输入（倍增扩容）
        size_t cap = 1 << 24, r;
        buf = (char *)malloc(cap + 1);
        len = 0;
        while ((r = fread(buf + len, 1, cap - len, stdin)) > 0) {
            len += r;
            if (len == cap) { cap *= 2; buf = (char *)realloc(buf, cap + 1); }
        }
        buf[len] = 0;
    }
    char *b, *e;
    ll x;
    nextLine(b, e);
    readInt(b, e, x);
    A = (int)x;
    N = A * A * A;
    g.assign(N + 1, 0);
    nb.assign((size_t)(N + 1) * 6, 0);
    deg.assign(N + 1, 0);
    // 邻接用定长数组（每点至多 6 个邻居），同时补反向边并去重，防止输入只给单向/有重复
    auto addArc = [&](int u, int w) {
        int *p = &nb[(size_t)u * 6];
        for (int k = 0; k < deg[u]; k++) if (p[k] == w) return;
        if (deg[u] < 6) p[deg[u]++] = w;
    };
    for (int i = 1; i <= N; i++) {
        nextLine(b, e);
        readInt(b, e, g[i]);
        while (readInt(b, e, x)) {
            int u = (int)x;
            if (u >= 1 && u <= N && u != i) { addArc(i, u); addArc(u, i); }
        }
    }
    // 找角
    int O = -1;
    for (int i = 1; i <= N; i++)
        if (deg[i] == 3) { O = i; break; }
    vector<int> dO = bfs(O);
    vector<int> axisEnd;
    for (int i = 1; i <= N; i++)
        if (deg[i] == 3 && dO[i] == A - 1) axisEnd.push_back(i);
    // axisEnd 应恰为 3 个
    vector<int> dX = bfs(axisEnd[0]), dY = bfs(axisEnd[1]), dZ = bfs(axisEnd[2]);
    vector<int> cx(N + 1), cy(N + 1), cz(N + 1);
    vector<int> grid(N, 0);
    auto gid = [&](int X, int Y, int Z) { return (X * A + Y) * A + Z; };
    for (int i = 1; i <= N; i++) {
        cx[i] = (dO[i] - dX[i] + A - 1) / 2;
        cy[i] = (dO[i] - dY[i] + A - 1) / 2;
        cz[i] = (dO[i] - dZ[i] + A - 1) / 2;
        grid[gid(cx[i], cy[i], cz[i])] = i;
    }
    // 光源与 6 个方向的光线
    const int DX[6] = {1, -1, 0, 0, 0, 0}, DY[6] = {0, 0, 1, -1, 0, 0}, DZ[6] = {0, 0, 0, 0, 1, -1};
    for (int i = 1; i <= N; i++) {
        if (g[i] != 0) continue;
        array<vector<int>, 6> rr;
        for (int d = 0; d < 6; d++) {
            int X = cx[i] + DX[d], Y = cy[i] + DY[d], Z = cz[i] + DZ[d];
            while (X >= 0 && X < A && Y >= 0 && Y < A && Z >= 0 && Z < A) {
                int c = grid[gid(X, Y, Z)];
                if (g[c] != 0) rr[d].push_back(c);  // 光源本身观赏值为 0，跳过不影响结果
                X += DX[d]; Y += DY[d]; Z += DZ[d];
            }
        }
        rays.push_back(rr);
    }
    L = (int)rays.size();
    cntLit.assign(N + 1, 0);
    dfs(0);
    printf("%lld %lld\n", best_min, best_max);
    return 0;
}
