/**
 * Author : RDP
 * There are no two words in the English language more harmful than "good job".
 * 1729 ;)
**/
#include <bits/stdc++.h>

using namespace std;
using ll = long long;

void debug_out() { cerr << '\n'; }
template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T)
{
    cerr << " " << to_string(H);
    debug_out(T...);
}

#define endl '\n'
#define debug(...) cerr << "[" << #__VA_ARGS__ << "]: ", debug_out(__VA_ARGS__)
#define FAST_IO              \
    ios::sync_with_stdio(0); \
    std::cin.tie(0);         \
    std::cout.tie(0);
#define all(x) (x).begin(), (x).end()
#define PI 3.1415926535897932384626433832795
const ll MOD = 1000000007;

ll sim_time, num_inters, num_streets, num_cars, bonus_point;

struct street
{
    int src, dest, time;
    string name;
};
struct car
{
    int path_size;
    vector<string> path;
};
struct intersection
{
    vector<string> incoming, outgoing;
    vector<int> lights;
};
vector<street> streets;
vector<car> cars;
vector<intersection> intersections;
map<string, int> used;
map<string, street> global_streets;
struct ans
{
    int id;
    vector<pair<string, int>> config;
};
bool cmp(const string &a, const string &b)
{
    return used[a] > used[b];
}
bool erase_zero_path(const string &s)
{
    return used[s] == 0;
}
void input()
{
    cin >> sim_time >> num_inters >> num_streets >> num_cars >> bonus_point;
    intersections.resize(num_inters);
    for (int i = 0; i < num_streets; i++)
    {
        street S;
        cin >> S.src >> S.dest >> S.name >> S.time;
        intersections[S.src].outgoing.push_back(S.name);
        intersections[S.dest].incoming.push_back(S.name);
        global_streets[S.name] = S;
    }
    for (int i = 0; i < num_cars; i++)
    {
        car C;
        cin >> C.path_size;
        C.path.resize(C.path_size);
        for (auto &s : C.path)
        {
            cin >> s;
            used[s]++;
        }
        cars.push_back(C);
    }
    for (auto inter : intersections)
        inter.incoming.erase(std::remove_if(inter.incoming.begin(), inter.incoming.end(), erase_zero_path), inter.incoming.end());
    return;
}
void print_ans(vector<ans> a)
{
    cout << a.size() << endl;
    for (auto x : a)
    {
        cout << x.id << endl;
        cout << x.config.size() << endl;
        for (auto p : x.config)
        {
            cout << p.first << ' ' << p.second << endl;
        }
    }
}
/*

D
D B A C
D B A


A B    C
1 0.95 0.95*0.95 

A -> 1
B -> 2
C -> 2
D -> 3
*/
//************************************************************//
vector<ans> strat_1()
{
    vector<ans> a;
    int i = 0;
    for (auto inter : intersections)
    {
        sort(all(inter.incoming), cmp);
        ans lcl_ans;
        lcl_ans.id = i;
        if (!inter.incoming.empty())
        {
            for (auto s : inter.incoming)
            {
                lcl_ans.config.push_back({s, rand() % 2 + 1});
            }
            if (!lcl_ans.config.empty())
            {
                a.push_back(lcl_ans);
            }
        }
        i++;
    }
    return a;
}
//*************************************************************************/
///////////////////////////////////////////////////////////////////////////////
map<string, double> strat_2_table;
bool strat_2_cmp(const string &a, const string &b)
{
    return strat_2_table[a] > strat_2_table[b];
}
vector<ans> strat_2(double initial = 1.0, double df = 0.95, double cutoff = 0.5, int upper_time = 2, int lower_time = 1)
{
    for (auto x : streets)
    {
        strat_2_table[x.name] = initial / x.time;
    }
    for (auto c : cars)
    {
        double coeff = initial;
        for (auto s : c.path)
        {
            strat_2_table[s] += coeff;
        }
        coeff *= df;
    }
    double mx = -1;
    for (auto &p : strat_2_table)
    {
        mx = max(mx, p.second);
    }
    for (auto &p : strat_2_table)
    {
        p.second /= mx;
    }
    vector<ans> a;
    int i = 0;
    for (auto inter : intersections)
    {
        sort(all(inter.incoming), strat_2_cmp);
        random_shuffle(all(inter.incoming));
        ans lcl_ans;
        lcl_ans.id = i;
        for (auto s : inter.incoming)
        {
            if (strat_2_table[s] >= cutoff)
                lcl_ans.config.push_back({s, rand() % 2 + 1});
            else
                lcl_ans.config.push_back({s, lower_time});
        }
        if (!lcl_ans.config.empty())
            a.push_back(lcl_ans);
        i++;
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
vector<ans> strat_3(double cutoff = 0.5)
{
    vector<ans> a;
    int i = 0;
    double sum = 0;
    for (auto &p : used)
    {
        sum += p.second;
    }
    for (auto inter : intersections)
    {
        sort(all(inter.incoming), cmp);
        random_shuffle(all(inter.incoming));
        ans lcl_ans;
        lcl_ans.id = i;
        if (!inter.incoming.empty())
        {
            for (auto s : inter.incoming)
            {
                if (used[s] / sum > cutoff)
                    lcl_ans.config.push_back({s, rand() % 2 + 1});
                else
                    lcl_ans.config.push_back({s, 1});
            }
            if (!lcl_ans.config.empty())
            {
                a.push_back(lcl_ans);
            }
        }
        i++;
    }
    return a;
}
//*************************************************************************/
//*************************************************************************/
map<string, ll> table_s4;
bool cmp_s4(const string &a, const string &b)
{
    return table_s4[a] < table_s4[b];
}

vector<ans> strat_4()
{
    vector<ans> a;
    for (auto c : cars)
    {
        ll cur = 0;
        for (auto x : c.path)
        {
            auto inter = global_streets[x];
            cur += inter.time;
            table_s4[x] += cur * used[x];
        }
    }
    int i = 0;
    for (auto inter : intersections)
    {
        sort(all(inter.incoming), cmp_s4);
        ans lcl_ans;
        lcl_ans.id = i;
        if (!inter.incoming.empty())
        {
            for (auto s : inter.incoming)
            {
                lcl_ans.config.push_back({s, 1});
            }
            if (!lcl_ans.config.empty())
            {
                a.push_back(lcl_ans);
            }
        }
        i++;
    }
    return a;
}
//*************************************************************************/
map<string, vector<ll>> table_s5;
map<string, double> t3;
map<string, ll> t2;
map<string, ll> t69;
bool cmp_s5(const string &a, const string &b)
{
    return t3[a] < t3[b];
}
vector<ans> strat_5()
{
    vector<ans> a;
    for (auto c : cars)
    {
        ll cur = 0;
        for (auto x : c.path)
        {
            table_s5[x].push_back(cur);
            t3[x] += cur;
            cur += global_streets[x].time;
        }
    }
    for (auto &p : table_s5)
    {
        sort(all(p.second));
        vector<ll> avg;
        for (int i = 1; i < p.second.size(); i++)
        {
            avg.push_back(abs(p.second[i] - p.second[i - 1]));
        }
        if (!avg.empty())
            t2[p.first] = accumulate(all(avg), 0LL) / avg.size();
        if (!avg.empty())
            t69[p.first] = avg.size();
    }
    int i = 0;
    for (auto inter : intersections)
    {
        random_shuffle(all(inter.incoming));
        ans lcl_ans;
        lcl_ans.id = i;
        if (!inter.incoming.empty())
        {
            for (auto s : inter.incoming)
            {
                if (t2.count(s) && t2[s] / t69[s] != 0)
                    lcl_ans.config.push_back({s, rand() % (t2[s] / t69[s]) + 1});
                else
                    lcl_ans.config.push_back({s, 1});
            }
            if (!lcl_ans.config.empty())
            {
                a.push_back(lcl_ans);
            }
        }
        i++;
    }
    return a;
}
//*************************************************************************/
void test_case()
{
    input();
    //print_ans(strat_1());
    //print_ans(strat_2(1000, 0.85, 0.90, 2, 1));
    //print_ans(strat_3());
    //print_ans(strat_4());
    print_ans(strat_5());

    return;
}
int main()
{
    //FAST_IO
    test_case();
    return 0;
}
/*
6 4 5 2 1000
2 0 rue-de-londres 1
0 1 rue-d-amsterdam 1
3 1 rue-d-athenes 1
2 3 rue-de-rome 2
1 2 rue-de-moscou 3
4 rue-de-londres rue-d-amsterdam rue-de-moscou rue-de-rome
3 rue-d-athenes rue-de-moscou rue-de-londres
*/
/*
void dummy()
{
    //last resort
    int cnt = 0;
    for (auto inter : intersections)
        if (!inter.incoming.empty())
            cnt++;
    cout << cnt << endl;
    int i = 0;
    for (auto inter : intersections)
    {
        if (!inter.incoming.empty())
        {
            cout << i << endl;
            cout << inter.incoming.size() << endl;
            random_shuffle(all(inter.incoming));
            for (auto s : inter.incoming)
                cout << s << " 1" << endl;
        }
        i++;
    }
}
*/