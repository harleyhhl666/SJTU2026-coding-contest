// T836182 2-3 连段练习
// 思路：每次询问 = 经典“最小覆盖子串”：在连段序列上双指针滑动窗口，
// 求包含目标列表 [l,r] 中所有不同操作的最短连续片段，O(m + n)。
// 结合数据范围：
//   - 测试点 11~15：q<=20，每次 O(m)，共 2e6；
//   - 测试点 16~20：n<=100，不同的 (l,r) 至多 5050 对，对 (l,r) 记忆化后共 5050*1e4=5e7。
// 字符串先离散化为整数编号；连段中不在目标列表里的操作只占长度，不参与计数，滑动时只遍历目标操作出现的位置。
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    unordered_map<string, int> idOf;
    idOf.reserve(4096);
    vector<int> tgt(n + 1);
    for (int i = 1; i <= n; i++) {
        string s;
        cin >> s;
        auto it = idOf.find(s);
        if (it == idOf.end()) {
            int id = (int)idOf.size();
            idOf.emplace(s, id);
            tgt[i] = id;
        } else tgt[i] = it->second;
    }
    int K = (int)idOf.size();
    int m;
    cin >> m;
    // 只保留在目标列表中出现过的操作的位置
    vector<int> P, PI;
    P.reserve(m);
    PI.reserve(m);
    for (int i = 1; i <= m; i++) {
        string s;
        cin >> s;
        auto it = idOf.find(s);
        if (it != idOf.end()) { P.push_back(i); PI.push_back(it->second); }
    }
    int q;
    cin >> q;
    vector<int> memo((size_t)(n + 1) * (n + 1), INT_MIN);
    vector<char> need(K, 0);
    vector<int> cnt(K, 0), needList;
    needList.reserve(K);
    // 每个目标操作在连段中是否出现
    vector<char> appear(K, 0);
    for (int id : PI) appear[id] = 1;

    string out;
    out.reserve((size_t)q * 8);
    int sz = (int)P.size();
    while (q--) {
        int l, r;
        cin >> l >> r;
        int &res = memo[(size_t)l * (n + 1) + r];
        if (res == INT_MIN) {
            needList.clear();
            bool ok = true;
            for (int i = l; i <= r; i++) {
                int id = tgt[i];
                if (!need[id]) { need[id] = 1; needList.push_back(id); if (!appear[id]) ok = false; }
            }
            int k = (int)needList.size();
            int best = INT_MAX;
            if (ok) {
                int have = 0, left = 0;
                for (int right = 0; right < sz; right++) {
                    int id = PI[right];
                    if (!need[id]) continue;
                    if (cnt[id]++ == 0) have++;
                    while (have == k) {
                        // 收缩左端：跳过不需要的位置，或计数>1 的冗余位置
                        int lid = PI[left];
                        if (!need[lid]) { left++; continue; }
                        if (cnt[lid] > 1) { cnt[lid]--; left++; continue; }
                        best = min(best, P[right] - P[left] + 1);
                        cnt[lid]--; have--; left++;
                    }
                }
            }
            for (int id : needList) { need[id] = 0; cnt[id] = 0; }
            res = (best == INT_MAX) ? -1 : best;
        }
        out += to_string(res);
        out += '\n';
    }
    cout << out;
    return 0;
}
