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
	void traverseTree(node* root);
	void staticSemantics(node* node);

	int varCount;
	std::vector< std::pair<std::string, std::string> > symbolTable; //<id, init_val>
	std::vector<int> scopedVarCounts;
	std::vector<std::string> scopedIdentifiers;

    void printAll(node* root);
    void printNode(node* node);
};

#endif