//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "TestTree.h"

void testPrintParseTree(ParseTree t) {
    std::cout << std::endl << "PARSE TREE:" << std::endl;
    t.printAll(t.root);
}