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
	root(NULL),
	symbolTable(std::vector< std::pair<std::string, std::string> >()),
	scopedVarCounts(std::vector<int>()),
	scopedIdentifiers(std::vector<std::string>())
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
	staticSemantics(root);
	for (int i = 0; i < root->children.size(); ++i) {
		if (root->children[i] != NULL) {
			traverseTree(root->children[i]);
		}
	}
}

void ParseTree::staticSemantics(node* node) {
	static bool isGlobal = true;
		
	if (isGlobal) {
		scopedVarCounts.push_back(0);
		isGlobal = false;
	}

	if (node->label.compare("vars") == 0) {

		//Calculate total number of declarations thus far in program
		int programTotalDeclarations = 0;
		for (int i = 0; i < scopedVarCounts.size(); ++i) {
			programTotalDeclarations += scopedVarCounts[i];
		}

		//Get number of vars in current scope
		int numVarsInCurrentScope = 0;
		if(scopedVarCounts.size() > 0)
			numVarsInCurrentScope = scopedVarCounts[scopedVarCounts.size() - 1];

		//Check if var already declared in current scope
		if (numVarsInCurrentScope > 0) {
			for (int i = programTotalDeclarations - numVarsInCurrentScope; i < programTotalDeclarations; ++i) {
				if (node->data[0].compare(scopedIdentifiers[i]) == 0) {
					std::cout << "ERROR: var "<< node->data[0] << " declared twice" << std::endl;
					exit(1);
				}
			}
		}

		symbolTable.push_back(std::pair<std::string, std::string>(node->data[0], node->data[1]));
		scopedIdentifiers.push_back(node->data[0]);
		scopedVarCounts[scopedVarCounts.size() - 1]++;
	}

	if (node->label.compare("block") == 0) {
		scopedVarCounts.push_back(0);
	}

	if (node->label.compare("endBlock") == 0) {
		for (int i = 0; i < scopedVarCounts[scopedVarCounts.size() - 1]; ++i) {
			scopedIdentifiers.pop_back();
		}
		scopedVarCounts.pop_back();
	}

	bool noMatch = true;

	if (node->label.compare("r") == 0 && node->data[0].compare("identifier") == 0) {
		for (int i = 0; i < scopedIdentifiers.size(); ++i) {
			if (node->data[1].compare(scopedIdentifiers[i]) == 0) {
				noMatch = false;
			}
		}

		if (noMatch) {
			std::cout << "ERROR: " << node->data[1] << " undeclared" << std::endl;
		}
	}

	if (node->label.compare("in") == 0) {
		for (int i = 0; i < scopedIdentifiers.size(); ++i) {
			if (node->data[1].compare(scopedIdentifiers[i]) == 0) {
				noMatch = false;
			}
		}

		if (noMatch) {
			std::cout << "ERROR: " << node->data[1] << " undeclared" << std::endl;
		}
	}

	if (node->label.compare("assign") == 0) {
		for (int i = 0; i < scopedIdentifiers.size(); ++i) {
			if (node->data[0].compare(scopedIdentifiers[i]) == 0) {
				noMatch = false;
			}
		}

		if (noMatch) {
			std::cout << "ERROR: " << node->data[0] << " undeclared" << std::endl;
		}
	}
}