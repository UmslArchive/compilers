//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "Table.h"

//Static inits
bool Table::isInitialized = false;

//Constructor
Table::Table() :
    sigma(std::map<char, int>()), 
    fsaTable(std::vector< std::vector<int> >(TOTAL_st, std::vector<int>()))
{       
        initSigma();
        buildFsaTable();

        isInitialized = true;
}

State Table::lookup(State state, char character) {
    char characterCategory;

    //Set category from character parameter for use in sigma:

    //digit
    if(character >= 48 && character <= 57)
        characterCategory = 'D';
    
    //alpha
    else if(character >= 97 && character <= 122)
        characterCategory = 'A';
    
    //whitespace
    else if(character == ' ' || character == '\t' || character == '\n')
        characterCategory = 'W';

    //otherOps
    else if(character == ':' || character == '+' || character == '-' || 
            character == '*' || character == '/' || character == '%' ||
            character == '.' || character == '(' || character == ')' ||
            character == ',' || character == '{' || character == '}' ||
            character == ';' || character == '[' || character == ']'    )
    {
        characterCategory = 'O';
    }
    //eof
    else if(character == 0)
        characterCategory = 'E';

    //specOps or invalid
    else
        characterCategory = character;
    
    //DEBUG
    /* std::cout << "sigSize" << sigma.size() << std::endl;
    std::cout << "charCat:" << characterCategory << " sigmaVal:" << sigma[characterCategory] << std::endl;  */

    //Check characterCategory validity (sigma[key] returns 0 if key not in map)
    if(sigma[characterCategory] == 0)
    {   
        //EOF character is not an error to report. Program just ends
        if(character != 0)
            std::cerr << "ERROR: Invalid character \'" << character << "\' ";

        return ERROR_st;
    }

    return (State)fsaTable [ state ] [ sigma[characterCategory] - 1 ];
}

void Table::initSigma() {
    //Start values at 1 because when using the [] operator with a std::map,
    //0 is returned if the key is not found.
    int value = 1;

    if(!isInitialized) {        
        //As the scanner reads in the file character by character, the character
        //will be categorized into one of these 7 categories. Ended up needing
        //to do this because otherwise the state transition table would've been
        //huge.
        
        //Special Operators
        sigma.insert(std::pair<char, int>('=', value++));
        sigma.insert(std::pair<char, int>('<', value++));
        sigma.insert(std::pair<char, int>('>', value++));

        //alpha
        sigma.insert(std::pair<char, int>('A', value++));

        //Other operators
        sigma.insert(std::pair<char, int>('O', value++));      

        //digits
        sigma.insert(std::pair<char, int>('D', value++));

        //Whitespace
        sigma.insert(std::pair<char, int>('W', value++));

        //EOF
        sigma.insert(std::pair<char, int>('E', value++));
    }
}

void Table::buildFsaTable() {
    if(!isInitialized) {

        //Allocate columns
        for(int i = 0; i < TOTAL_st; ++i) {
            fsaTable[i] = std::vector<int>(sigma.size());
        }

        //Add state transitions:

        //Start    
        fsaTable[START_st][sigma['='] - 1] = ASSIGN_st;
        fsaTable[START_st][sigma['<'] - 1] = LT_st;
        fsaTable[START_st][sigma['>'] - 1] = GT_st;
        fsaTable[START_st][sigma['A'] - 1] = MID_ID_st;
        fsaTable[START_st][sigma['O'] - 1] = MID_OP_st;
        fsaTable[START_st][sigma['D'] - 1] = MID_INT_st;
        fsaTable[START_st][sigma['W'] - 1] = START_st;
        fsaTable[START_st][sigma['E'] - 1] = FIN_EOF_st;

        //Mid identifier
        fsaTable[MID_ID_st][sigma['='] - 1] = FIN_IDENT_st;
        fsaTable[MID_ID_st][sigma['<'] - 1] = FIN_IDENT_st;
        fsaTable[MID_ID_st][sigma['>'] - 1] = FIN_IDENT_st;
        fsaTable[MID_ID_st][sigma['A'] - 1] = MID_ID_st;
        fsaTable[MID_ID_st][sigma['O'] - 1] = FIN_IDENT_st;
        fsaTable[MID_ID_st][sigma['D'] - 1] = MID_ID_st;
        fsaTable[MID_ID_st][sigma['W'] - 1] = FIN_IDENT_st;
        fsaTable[MID_ID_st][sigma['E'] - 1] = FIN_IDENT_st;

        //Mid integer
        fsaTable[MID_INT_st][sigma['='] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['<'] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['>'] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['A'] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['O'] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['D'] - 1] = MID_INT_st;
        fsaTable[MID_INT_st][sigma['W'] - 1] = FIN_INT_st;
        fsaTable[MID_INT_st][sigma['E'] - 1] = FIN_INT_st;
        
        //Mid operator
        fsaTable[MID_OP_st][sigma['='] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[MID_OP_st][sigma['E'] - 1] = FIN_OP_st;

        //Greater than operator
        fsaTable[GT_st][sigma['='] - 1] = GT_EQ_st;
        fsaTable[GT_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[GT_st][sigma['E'] - 1] = FIN_OP_st;

        //Less than operator
        fsaTable[LT_st][sigma['='] - 1] = LT_EQ_st;
        fsaTable[LT_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[LT_st][sigma['E'] - 1] = FIN_OP_st;


        //Assignment operator
        fsaTable[ASSIGN_st][sigma['='] - 1] = EQ_st;
        fsaTable[ASSIGN_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[ASSIGN_st][sigma['E'] - 1] = FIN_OP_st;

        //Equality comparison operator
        fsaTable[EQ_st][sigma['='] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[EQ_st][sigma['E'] - 1] = FIN_OP_st;

        //Less than equal comparison operator
        fsaTable[LT_EQ_st][sigma['='] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[LT_EQ_st][sigma['E'] - 1] = FIN_OP_st;

        //Greater than equal comparison operator
        fsaTable[GT_EQ_st][sigma['='] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['<'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['>'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['A'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['O'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['D'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['W'] - 1] = FIN_OP_st;
        fsaTable[GT_EQ_st][sigma['E'] - 1] = FIN_OP_st;
    }
}

