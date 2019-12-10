//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------
#include "TestScanner.h"

//Constructor
TestScanner::TestScanner()
{}

//Primary scanner test
void TestScanner::primaryScannerTest(std::string fileData) {
    std::string mockFile = fileData;
    scanner.loadFileData(mockFile);

    bool finished = false;
    while(!finished) {
        Token currentToken = scanner.getNextToken();

        currentToken.printToken();

        if(currentToken.getID() == EOF_tk) {
            finished = true;
        }

        if(currentToken.getID() == ERROR_tk) {
            finished = true;
        }
    }
}

//Run all token tests
void TestScanner::runTokenTests() {
    testTokenKeywords();
    testPrintToken();
}

//Test to ensure that static member keywords is being initialized only once
void TestScanner::testTokenKeywords() {

    //Arbitrarily declare 3 tokens
    Token a(IDENTIFIER_tk, "TokenA", 3);
    Token b(INTEGER_tk, "TokenB", 4);
    Token c(EOF_tk, "TokenC", 5);

    //Print Test info
    std::cout << "\nTEST: Token keywords vector is static" << std::endl <<
    "Should see 3 sets of the same words separated by an extra newline." << 
    std::endl << "---------" << std::endl;

    //Print each token's keyword string vector
    std::cout << a.getInstance() << ": ";
    a.printKeywordVector();
    std::cout << b.getInstance() << ": ";
    b.printKeywordVector();
    std::cout << c.getInstance() << ": ";
    c.printKeywordVector();
    std::cout << "=========" << std::endl;
    std::cout << "=========" << std::endl;

    return;
}

void TestScanner::testPrintToken() {
    //Print test info
    std::cout << "\nTEST: Tokens are properly being assigned values\n---------\n";

    //Arbitrarily declare 3 tokens
    Token a(IDENTIFIER_tk, "TokenA", 3);
    Token b(INTEGER_tk, "TokenB", 4);
    Token c(EOF_tk, "TokenC", 5);

    a.printToken();
    b.printToken();
    c.printToken();
    std::cout << std::endl;
}

