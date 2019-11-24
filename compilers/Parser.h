//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#include "Scanner.h"
#include "ParseTree.h"

class Parser  {
private:
    static const bool debug = false;
    Scanner scanner;
    Token currentToken;
    ParseTree parseTree;

    //Nonterminal functions: 

    node* program();
    node* block();
    node* vars();
    node* expr();
    node* Z();
    node* A();
    node* Y();
    node* N();
    node* X();
    node* M();
    node* R();
    node* stats();
    node* mStat();
    node* stat();
    node* in();
    node* out();
    node* IF();
    node* loop();
    node* assign();
    node* RO();
    node* W();
    node* T();

    void error();

public:
    //Constuctor
    Parser();

    //Init
    void sendFileDataToScanner(std::string fileData);

    //Runs the recursive decent parser to completion and returns the generated tree
    ParseTree parse();

};

#endif