//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "ParseTree.h"

node::node(std::string label) :
    label(label), 
    children(std::vector<node*>()),
    data(std::vector<std::string>())
{
    //std::cout << label << children.size() << data.size() << std::endl;
}

ParseTree::ParseTree() :
    indent(0), 
	varCount(0),
	root(NULL)
{}

void ParseTree::printNode(node* node) {
    for(int i = 0; i < indent; ++i) {
        std::cout << " ";
    }

    std::cout << node->label;

    if(node->data.size() > 0)
        std::cout << "\tdata: ";

    for(int i = 0; i < node->data.size(); ++i) {
        std::cout << node->data[i] << ", ";
    }

    std::cout << std::endl;
}

void ParseTree::printAll(node* root) {
    printNode(root);

    for(int i = 0; i < root->children.size(); ++i) {
        indent += 2;
        if(root->children[i] != NULL) {
            printAll(root->children[i]);
        }
        indent -= 2;
    }
}

void ParseTree::traverseTree(node* root) {
	for (int i = 0; i < root->children.size(); ++i) {
		if (root->children[i] != NULL) {
			traverseTree(root->children[i]);
		}
	}
}

void ParseTree::processNode(node* node) {
    
}