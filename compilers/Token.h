//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

enum TokenID {
    INTEGER_tk,
    IDENTIFIER_tk,
    EOF_tk,
    OPERATOR_tk,

    //Keywords
    KWD_START_tk,
    KWD_STOP_tk,
    KWD_ITERATE_tk,
    KWD_VOID_tk,
    KWD_VAR_tk,
    KWD_RETURN_tk,
    KWD_IN_tk,
    KWD_OUT_tk,
    KWD_PROGRAM_tk,
    KWD_COND_tk,
    KWD_THEN_tk,
    KWD_LET_tk,
    
    TOTAL_tk,

    ERROR_tk
};

class Token {
private:
    TokenID id;
    int lineNum;
    std::string instance;

    //Keyword lookup for when IDENTIFIER_tk is found
    static bool isInitialized;
    static std::vector<std::string> keywords;

public:
    //Constructor
    Token(TokenID id, std::string instance, int lineNum);

    //Accessors
    TokenID getID();
    std::string idName(int id);
    int getLineNum();
    std::string getInstance();

    //Utility
    std::string findKeyword(std::string checkString);

    //Debug
    void printToken();
    void printKeywordVector();
};

#endif