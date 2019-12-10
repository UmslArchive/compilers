//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "TestParser.h"

void TestParser::parserNoTreeTest(std::string fileData) {
    
    parser.sendFileDataToScanner(fileData);

    parser.parse();
}