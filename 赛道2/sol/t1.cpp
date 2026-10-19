// T836149 2-1 守包落位
// 思路：输入按“行”给出邻接点（不给个数），所以整体读入后逐行解析。
// 以 1 为根做 DFS 序（迭代，避免链状树爆栈），子树 = 连续区间 [tin, tout]。
// 线段树维护区间和与区间最大值，支持单点修改。O((n+q) log n)。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char *buf;
static size_t len, pos_ = 0;

// 读取下一行（跳过空行），返回 [b,e) 指针
bool nextLine(char *&b, char *&e) {
    while (pos_ < len) {
        size_t s = pos_;
        while (pos_ < len && buf[pos_] != '\n') pos_++;
        size_t t = pos_;
        if (pos_ < len) pos_++;
        // 去掉行尾 \r 和空白
        bool nonblank = false;
        for (size_t i = s; i < t; i++)
            if (!isspace((unsigned char)buf[i])) { nonblank = true; break; }
        if (nonblank) { b = buf + s; e = buf + t; return true; }
    }
    return false;
}
// 从 [p,e) 中解析一个整数，失败返回 false
bool readInt(char *&p, char *e, ll &x) {
    while (p < e && !(*p == '-' || (*p >= '0' && *p <= '9'))) p++;
    if (p >= e) return false;
    bool neg = false;
    if (*p == '-') { neg = true; p++; }
    x = 0;
    while (p < e && *p >= '0' && *p <= '9') x = x * 10 + (*p++ - '0');
    if (neg) x = -x;
    return true;
}

int n, q;
vector<ll> sm, mx;
int sz;
void build(int node, int l, int r, const vector<ll> &a) {
    if (l == r) { sm[node] = mx[node] = a[l]; return; }
    int m = (l + r) / 2;
    build(node * 2, l, m, a);
    build(node * 2 + 1, m + 1, r, a);
    sm[node] = sm[node * 2] + sm[node * 2 + 1];
    mx[node] = max(mx[node * 2], mx[node * 2 + 1]);
}
void update(int node, int l, int r, int p, ll v) {
    if (l == r) { sm[node] = mx[node] = v; return; }
    int m = (l + r) / 2;
    if (p <= m) update(node * 2, l, m, p, v);
    else update(node * 2 + 1, m + 1, r, p, v);
    sm[node] = sm[node * 2] + sm[node * 2 + 1];
    mx[node] = max(mx[node * 2], mx[node * 2 + 1]);
}
ll qs(int node, int l, int r, int ql, int qr) {
    if (ql <= l && r <= qr) return sm[node];
    int m = (l + r) / 2;
    ll s = 0;
    if (ql <= m) s += qs(node * 2, l, m, ql, qr);
    if (qr > m) s += qs(node * 2 + 1, m + 1, r, ql, qr);
    return s;
}
ll qm(int node, int l, int r, int ql, int qr) {
    if (ql <= l && r <= qr) return mx[node];
    int m = (l + r) / 2;
    ll s = LLONG_MIN;
    if (ql <= m) s = max(s, qm(node * 2, l, m, ql, qr));
    if (qr > m) s = max(s, qm(node * 2 + 1, m + 1, r, ql, qr));
    return s;
}

int main() {
    // 读入全部输入
    {
        vector<char> v;
        char tmp[1 << 16];
        size_t r;
        while ((r = fread(tmp, 1, sizeof tmp, stdin)) > 0) v.insert(v.end(), tmp, tmp + r);
        len = v.size();
        buf = (char *)malloc(len + 1);
        memcpy(buf, v.data(), len);
        buf[len] = 0;
    }
    char *b, *e;
    ll x;
    nextLine(b, e);
    readInt(b, e, x); n = (int)x;
    readInt(b, e, x); q = (int)x;

    vector<ll> val(n + 1);
    vector<vector<int>> adj(n + 1);
    for (int i = 1; i <= n; i++) {
        nextLine(b, e);
        readInt(b, e, val[i]);
        while (readInt(b, e, x)) {
            int u = (int)x;
            if (u >= 1 && u <= n && u != i) { adj[i].push_back(u); adj[u].push_back(i); }
        }
    }
    // 迭代 DFS 求 tin/tout（邻接表中可能重复，用 visited 去重）
    vector<int> tin(n + 1), tout(n + 1), it(n + 1, 0);
    vector<char> vis(n + 1, 0);
    vector<int> st;
    st.reserve(n);
    int timer = 0;
    st.push_back(1); vis[1] = 1; tin[1] = ++timer;
    while (!st.empty()) {
        int u = st.back();
        if (it[u] < (int)adj[u].size()) {
            int w = adj[u][it[u]++];
            if (!vis[w]) { vis[w] = 1; tin[w] = ++timer; st.push_back(w); }
        } else {
            tout[u] = timer;
            st.pop_back();
        }
    }
    vector<ll> a(n + 1);
    for (int i = 1; i <= n; i++) a[tin[i]] = val[i];
    sz = n;
    sm.assign(4 * n + 4, 0);
    mx.assign(4 * n + 4, LLONG_MIN);
    build(1, 1, n, a);

    string out;
    out.reserve(q * 12);
    // 事件部分：逐个 token 读取即可
    char *p = buf + pos_, *E = buf + len;
    for (int i = 0; i < q; i++) {
        ll t, u;
        if (!readInt(p, E, t)) break;
        readInt(p, E, u);
        if (t == 1) {
            readInt(p, E, x);
            update(1, 1, n, tin[u], x);
        } else if (t == 2) {
            out += to_string(qs(1, 1, n, tin[u], tout[u]));
            out += '\n';
        } else {
            out += to_string(qm(1, 1, n, tin[u], tout[u]));
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
