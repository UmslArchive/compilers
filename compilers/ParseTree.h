//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <cstdlib>
#include <iterator>

struct node {
    std::string label;
    std::vector<node*> children;
    std::vector<std::string> data;

    //Constructor
    node(std::string label);
};

class ParseTree {
public:
    ParseTree();

    node* root;

    int indent;

	//Static semantics
	void staticSemanticsTraversal(node* root);
	void staticSemantics(node* node);
	std::vector< std::pair<std::string, std::string> > symbolTable; //<id, init_val>
	std::vector<int> scopedVarCounts;
	std::vector<std::string> scopedIdentifiers;

	//ASM Code generation
	void codeGenTraversal(node* root);
	void generateASM(node* anode);
	void getExprString(node* exprNode);
	void evaluateExpression();
	std::vector< std::vector<std::string> > exprStack;
	std::vector<std::string> exprString;

	bool tokenIsIdentifier(std::string token);

	int skipCount;
	int tempCount;
	//void evaluateExpression(node* node, std::vector<std::string>& exprString);
	//void manualOverride(std::vector<std::string>& exprResult);

	void printUsedVars();

    void printAll(node* root);
    void printNode(node* node);

};

#endif