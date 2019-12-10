//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <map>
#include <vector>

//Rows
enum State {
    START_st,
    MID_ID_st,
    MID_INT_st,
    MID_OP_st,
    GT_st,
    LT_st,
    ASSIGN_st,
    EQ_st,
    LT_EQ_st,
    GT_EQ_st,

    TOTAL_st, //Total number of non-final states
     
    FIN_EOF_st,
    FIN_IDENT_st,
    FIN_INT_st,
    FIN_OP_st,
    ERROR_st
};

class Table {
private:
    static bool isInitialized;
    
    //Column mapping
    std::map<char, int> sigma;

    //2D table (vector of vectors)
    std::vector< std::vector<int> > fsaTable;

    void buildFsaTable();
    void initSigma();

public:
    //Constructor
    Table();

    State lookup(State state, char character);

    void testLookup();
};

#endif