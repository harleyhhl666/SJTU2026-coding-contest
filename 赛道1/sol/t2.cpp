// T836895 1-2 整除问题
// 1) m | N：按 r 进制 Horner 求 N mod m。
// 2) 系数 k：x = rq + d，r(q + kd) = x + (rk - 1)d。gcd(r,m)=1，r 可逆，
//    故“m|x ⇔ m|(q+kd) 对所有 x 成立” ⇔ rk ≡ 1 (mod m)，k = r^{-1} mod m（m=1 时 k=0）。
// 3) 序列：F(x) = q + kd 在 r 进制下就是“删掉末位，再加上 k*d”（k*d <= 19999*35 < 7e5），
//    用低位在前的数字数组做，每步均摊 O(1)。当 x 很大时 x - F(x) = (r-1)q - (k-1)d > 0，
//    严格递减、不会重复，只需与 T 比较；位数降到 <=12 位（< 36^12 ≈ 4.7e18，可放进 long long）后
//    改用整数迭代 + 哈希集合判重。小数阶段的值满足 F(x) <= x/r + (m-1)(r-1)，最终落入 < m*r 的范围，
//    步数有界。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int val(char c) { return (c >= '0' && c <= '9') ? c - '0' : c - 'A' + 10; }

// 读入并转成低位在前、去掉前导零的数组（0 表示为空数组）
vector<int> parse(const string &s) {
    vector<int> d;
    for (int i = (int)s.size() - 1; i >= 0; i--) d.push_back(val(s[i]));
    while (!d.empty() && d.back() == 0) d.pop_back();
    return d;
}

int main() {
    ll r, m;
    string sN, sT;
    cin >> r >> m >> sN >> sT;

    // 1) N mod m
    ll mod = 0;
    for (char c : sN) mod = (mod * r + val(c)) % m;
    puts(mod == 0 ? "Yes" : "No");

    // 2) k
    ll k = 0;
    if (m > 1) {
        for (ll t = 1; t < m; t++)
            if (r * t % m == 1) { k = t; break; }
    }
    printf("%lld\n", k);

    // 3) 序列
    vector<int> x = parse(sN), T = parse(sT);
    // SMALL = 满足 r^SMALL <= 4e18 的最大位数（r=2 时 61 位，r=36 时 11 位）。
    // 位数 > SMALL 时 q >= r^(SMALL-1) >= 4e18/r^2 > 3e15 远大于 (k-1)d，保证严格递减；
    // 位数 <= SMALL 时值 < r^SMALL <= 4e18，整数运算不溢出。
    int SMALL = 0;
    {
        ll p = 1;
        while (p <= (ll)4e18 / r) { p *= r; SMALL++; }
    }
    auto toLL = [&](const vector<int> &a) {
        ll v = 0;
        for (int i = (int)a.size() - 1; i >= 0; i--) v = v * r + a[i];
        return v;
    };
    bool found = false;
    // 大数阶段：严格递减，不会出现重复
    while ((int)x.size() > SMALL) {
        if (x == T) found = true;
        // F(x)：删末位（低位在前，即删 x[0]），再加 k*d
        ll d = x[0];
        x.erase(x.begin());  // O(L)，总计 O(L^2)=9e6，可接受
        ll carry = k * d;
        for (size_t i = 0; carry > 0; i++) {
            if (i == x.size()) x.push_back(0);
            ll s = x[i] + carry;
            x[i] = (int)(s % r);
            carry = s / r;
        }
    }
    // 小数阶段
    bool Tsmall = (int)T.size() <= SMALL;
    ll tv = Tsmall ? toLL(T) : -1;
    ll cur = toLL(x);
    unordered_set<ll> seen;
    seen.reserve(1 << 20);
    while (!seen.count(cur)) {
        seen.insert(cur);
        if (cur == tv) found = true;
        cur = cur / r + k * (cur % r);
    }
    puts(found ? "Yes" : "No");
    return 0;
}
