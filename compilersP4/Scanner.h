//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef SCANNER_H
#define SCANNER_H

#include <limits>

#include "Table.h"
#include "Token.h"

class Scanner {
private:
    //Finite State Automaton transition table
    static Table FSA;
    State currentState;
    
    //File data to be parsed.
    std::string fileData;
    int lineCount;

    //Stream objects
    char currentChar;
    std::stringstream fileDataParser;

    char readNextCharacter();
    
public:
    Scanner();
    Token getNextToken();
    void loadFileData(std::string fileData);
};

#endif