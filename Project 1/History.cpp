//
//  History.cpp
//  Project 1
//
//  Created by Cecilia.Wang on 1/7/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//


#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

#include "History.h"

History::History(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
    
    for (int i = 1; i<= m_rows; i++){
        for (int j = 1; j <= m_cols; j++){
           grid[i][j] = '.';
        }
    }
}

bool History::record(int r, int c)
{
    if (r < 1 || r > m_rows || c < 1 || c > m_cols)
        return false;
    
    if (grid[r][c] == '.')
        grid[r][c] = 'A';
    else if (grid[r][c] < 'Z')
        grid[r][c]++;
    return true;
}

void History:: display() const
{
    clearScreen();
    for (int i = 1; i<= m_rows; i++){
        for (int j = 1; j <= m_cols; j++){
            cout << grid[i][j];
        }
        cout<< endl;
    }
    cout << endl;
}

