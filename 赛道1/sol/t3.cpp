// T836868 1-3 套包
// 只与出现过的“不同规格值”集合 S 有关。把 S 划分成若干段极长连续整数段 [s,e]：
//   - 同一段内被选的两个值相差 >= 2，段长为 len 时最多选 ceil(len/2) 个（取 s, s+2, ...）；
//   - 相邻两段之间至少空一个整数，前一段选的最大值 <= e，后一段起点 >= e+2，互不冲突。
// 所以答案 = Σ ceil(len/2)。在线插入新值 v 时，它把左侧以 v-1 结尾的段（长 L）和
// 右侧以 v+1 开头的段（长 R）合并成长 L+R+1 的段，答案增量 O(1)。
// 段长只记录在段的两个端点上：len[左端点] = len[右端点] = 段长。复杂度 O(n + maxA)。
#include <bits/stdc++.h>
using namespace std;

const int MAXV = 500005;
int lenAt[MAXV + 2];
bool has[MAXV + 2];

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    long long ans = 0;
    string out;
    out.reserve((size_t)n * 7);
    auto half = [](int l) { return (l + 1) / 2; };
    for (int i = 0; i < n; i++) {
        int v;
        scanf("%d", &v);
        if (!has[v]) {
            has[v] = true;
            int L = (v - 1 >= 1 && has[v - 1]) ? lenAt[v - 1] : 0;
            int R = has[v + 1] ? lenAt[v + 1] : 0;
            ans -= half(L) + half(R);
            int len = L + R + 1;
            ans += half(len);
            lenAt[v - L] = len;
            lenAt[v + R] = len;
        }
        if (i) out += ' ';
        out += to_string(ans);
    }
    out += '\n';
    fputs(out.c_str(), stdout);
    return 0;
}
