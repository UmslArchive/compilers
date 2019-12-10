//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "Scanner.h"

//Static init
Table Scanner::FSA = Table();

//Constructor
Scanner::Scanner() :
    fileData(""), 
    lineCount(1)
{}


Token Scanner::getNextToken() {

    //Used to fetch the 
    Token dummyToken(ERROR_tk, "DUMMY", -1);

    //Token attributes
    TokenID tid;
    std::string tokenInstance = "";

    //Bool to store if first call to this function
    static bool first = true;

    //Reset state
    currentState = START_st;

    //Get first character of file
    if(first) {
        currentChar = readNextCharacter();
        first = false;
    }
        
    int count = 0;

    //Loop until a finish state is reached
    while(1) {
       
        //Check table
        currentState = FSA.lookup(currentState, currentChar);

        //DEBUG
        //std::cout << "currentState:" << currentState << " currentChar: " << (int)currentChar <<  "(" << currentChar << ")" << std::endl;

        //Check for a final state:
        if(currentState == FIN_EOF_st) {
            return Token(EOF_tk, "EOF", lineCount);
        }

        if(currentState == FIN_IDENT_st) {
            tid = IDENTIFIER_tk;

            //Check for keyword match
            if(dummyToken.findKeyword(tokenInstance) != "NO_MATCH") {
                if(tokenInstance.compare("start") == 0)
                    tid = KWD_START_tk;
                
                if(tokenInstance.compare("stop") == 0)
                    tid = KWD_STOP_tk;
                
                if(tokenInstance.compare("iterate") == 0)
                    tid = KWD_ITERATE_tk;
                
                if(tokenInstance.compare("void") == 0)
                    tid = KWD_VOID_tk;

                if(tokenInstance.compare("var") == 0)
                    tid = KWD_VAR_tk;

                if(tokenInstance.compare("return") == 0)
                    tid = KWD_RETURN_tk;
                
                if(tokenInstance.compare("in") == 0)
                    tid = KWD_IN_tk;

                if(tokenInstance.compare("out") == 0)
                    tid = KWD_OUT_tk;

                if(tokenInstance.compare("program") == 0)
                    tid = KWD_PROGRAM_tk;

                if(tokenInstance.compare("cond") == 0)
                    tid = KWD_COND_tk;

                if(tokenInstance.compare("then") == 0)
                    tid = KWD_THEN_tk;

                if(tokenInstance.compare("let") == 0)
                    tid = KWD_LET_tk;
            }

            break;
        }

        if(currentState == FIN_OP_st ) {
            tid = OPERATOR_tk;
            break;
        }

        if(currentState == FIN_INT_st) {
            tid = INTEGER_tk;
            break;
        }

        if(currentState == ERROR_st) {
            std::cerr << "on line #" << lineCount << std::endl;
            return Token(ERROR_tk, "ERROR_TOKEN", lineCount);
        }


        //Only push onto tokenInstance string if exited start state
        if(currentState != START_st) {
            tokenInstance.push_back(currentChar);
        }

        //Read 
        currentChar = readNextCharacter();

        //Ignore comments
        while(currentChar == -1) {
            //std::cout << "";
            currentChar = readNextCharacter();
        }


    }
    
    //If a token happens to be next to a newline linecount gets incremented
    //too early. This resolves that issue.
    if(currentChar == '\n')
        return Token(tid, tokenInstance , lineCount - 1);
    
    return Token(tid, tokenInstance , lineCount);

}

char Scanner::readNextCharacter() {
    char c;
    if(fileDataParser.get(c)) {
        //fileDataParser.get(c);

        //Ignore newlines
        if(c == '\n') {
            lineCount++;
            //return -1;
        }

        //Ignore comments
        if(c == '#') {
            fileDataParser.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            lineCount++;

            return -1;
        }

        return c;
   }
    
   return 0;
    
}

void Scanner::loadFileData(std::string fileData) {
    this->fileData = fileData;
    fileDataParser.str(this->fileData);
}