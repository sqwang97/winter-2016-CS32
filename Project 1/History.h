//
//  History.hpp
//  Project 1
//
//  Created by Cecilia.Wang on 1/7/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#ifndef History_h
#define History_h

#include "globals.h"

class History
{
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;
    
private:
    int     m_rows;
    int     m_cols;
    char    grid[MAXROWS][MAXCOLS];
};

#endif /* History_h */
