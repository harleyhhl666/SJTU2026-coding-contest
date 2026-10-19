// T836214 1-1 物资分配 —— 0/1 背包
// dp[j] = 容量不超过 j 时的最大价值；倒序枚举容量保证每件只用一次。
// v_i 最大 1e9，n=1000 件总和可达 1e12，需 long long。重量超过 C 的物品直接跳过。
// 复杂度 O(nC) = 1e6。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    int n, C;
    if (scanf("%d %d", &n, &C) != 2) return 0;
    vector<ll> dp(C + 1, 0);
    for (int i = 0; i < n; i++) {
        int w;
        ll v;
        scanf("%d %lld", &w, &v);
        if (w > C) continue;
        for (int j = C; j >= w; j--)
            dp[j] = max(dp[j], dp[j - w] + v);
    }
    printf("%lld\n", dp[C]);
    return 0;
}
