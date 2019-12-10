//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include "Parser.h"

class TestParser {
private: 
    Parser parser;

public:
    void parserNoTreeTest(std::string fileData);
};

#endif