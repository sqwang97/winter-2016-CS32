//
//  main.cpp
//  Project 4
//
//  Created by Cecilia.Wang on 2/29/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <set>
#include "DiskMultiMap.h"

using namespace std;

#define N 10000

struct node
{
    string key, val, con;
    
    node(){};
    node(const string &key_, const string &val_, const string &con_)
    :key(key_), val(val_), con(con_)
    {}
    
    bool operator ==(const node &B) const
    {
        return key == B.key && val == B.val && con == B.con;
    }
    
    bool operator <(const node &B) const
    {
        if (key != B.key)
            return key < B.key;
        if (val != B.val)
            return val < B.val;
        if (con != B.con)
            return con < B.con;
        return false;
    }
};

DiskMultiMap DM;
multiset<node> S;

node A[N];

void randStr(string &s)
{
    s = "";
    for (int i = 0; i < 100; i++)
        s += rand() % 26 + 'A';
}


void mutation()
{
    for (int i = 0; i < N; i++)
        if (!(i % (N >> 10)))
        {
            randStr(A[i].val);
            randStr(A[i].con);
        }
}

void insert()
{
    for (int i = 0; i < N; i++)
    {
        S.insert(A[i]);
        DM.insert(A[i].key, A[i].val, A[i].con);
    }
}

void erase()
{
    int k, a, b;
    for (int i = 0; i < (N >> 4); i++)
    {
        k = rand() % N;
        if ((a = DM.erase(A[k].key, A[k].val, A[k].con)) != (b = (int)S.erase(A[k])))
        {
            printf("%d %d: ERASE ERROR!\n", a, b);
            exit(1);
        }
    }
}

void clean()
{
    int a, b;
    for (int i = 0; i < N; i++)
        if ((a = DM.erase(A[i].key, A[i].val, A[i].con)) != (b = (int)S.erase(A[i])))
        {
            printf("%d %d: ERASE ERROR!\n", a, b);
            exit(1);
        }
}

void run()
{
    for (int i = 0; i < 10; i++)
    {
        DM.openExisting("TEST");
        mutation();
        insert();
        erase();
        DM.close();
    }
    DM.openExisting("TEST");
    clean();
}

void init()
{
    for (int i = 0; i < (N >> 3); i++)
    {
        randStr(A[i].key);
        randStr(A[i].val);
        randStr(A[i].con);
    }
    
    for (int i = (N >> 3); i < N; i++)
        A[i] = A[rand() % i];
    
    for (int i = 0; i < (N >> 2); i++)
        swap(A[rand() % N], A[rand() % N]);
}

int main()
{
    DM.createNew("TEST", N >> 1);
    init();
    run();
    DM.close();
    printf("Passed Randomized Test.\n");
    return 0;
}