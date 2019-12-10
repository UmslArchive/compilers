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
	root(NULL),
	symbolTable(std::vector< std::pair<std::string, std::string> >()),
	scopedVarCounts(std::vector<int>()),
	scopedIdentifiers(std::vector<std::string>()),
	exprStack(1, std::vector<std::string>()),
	skipCount(0),
	tempCount(0),
	exprString(std::vector<std::string>()),
	loopCount(0),
	outFileName("defaultOutFile.asm")
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

void ParseTree::openOutFileStream() {
	this->fout.open(outFileName + ".asm", std::fstream::out | std::fstream::trunc);
}

void ParseTree::closeOutFileStream() {
	this->fout.close();
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

void ParseTree::staticSemanticsTraversal(node* root) {
	staticSemantics(root);
	for (int i = 0; i < root->children.size(); ++i) {
		if (root->children[i] != NULL) {
			staticSemanticsTraversal(root->children[i]);
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
					std::cout << "ERROR: var "<< node->data[0] << " declared multiple times" << std::endl;
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
			std::cout << "ERROR: identifier '" << node->data[1] << "' undeclared" << std::endl;
			exit(1);
		}
	}

	if (node->label.compare("in") == 0) {
		for (int i = 0; i < scopedIdentifiers.size(); ++i) {
			if (node->data[1].compare(scopedIdentifiers[i]) == 0) {
				noMatch = false;
			}
		}

		if (noMatch) {
			std::cout << "ERROR: identifier '" << node->data[1] << "' undeclared" << std::endl;
			exit(1);
		}
	}

	if (node->label.compare("assign") == 0) {
		for (int i = 0; i < scopedIdentifiers.size(); ++i) {
			if (node->data[0].compare(scopedIdentifiers[i]) == 0) {
				noMatch = false;
			}
		}

		if (noMatch) {
			std::cout << "ERROR: identifier '" << node->data[0] << "' undeclared" << std::endl;
			exit(1);
		}
	}
}

void ParseTree::codeGenTraversal(node* root) {
	generateASM(root);
	for (int i = 0; i < root->children.size(); ++i) {
		if (root->children[i] != NULL) {
			codeGenTraversal(root->children[i]);
		}
	} 
}

void ParseTree::generateASM(node* node) {
	std::string lhs = "";
	std::string rhs = "";

	if (node->label.compare("out") == 0) {
		getExprString(node->children[0]);
		evaluateExpression();

		if (tokenIsIdentifier(exprString[0]) && exprString.size() == 1) {
			std::cout << "WRITE " << exprString[0] << std::endl;
		}
		else if(exprString.size() < 2) {
			std::cout << "LOAD " << exprString[0] << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "WRITE TEMP" << tempCount << std::endl;
			tempCount++;
		}
		else {
			std::cout << "WRITE TEMP001" << std::endl;
		}
		exprString.clear();
	}

	if (node->label.compare("in") == 0) {
		std::cout << "READ " << node->data[1] << std::endl;

		//Update symbol table
		for (int i = 0; i < symbolTable.size(); ++i) {
			if (symbolTable[i].first.compare(node->data[1]) == 0) {

			}
		}
	}

	if (node->label.compare("if") == 0) {
		//Evaluate expressions:

		//LHS
		getExprString(node->children[0]);
		evaluateExpression();
		lhs = exprString[0];
		exprString.clear();

		//RHS
		getExprString(node->children[2]);
		evaluateExpression();
		rhs = exprString[0];
		exprString.clear();
		
		//Get relational operator
		std::string relate = "";
		relate.append(node->children[1]->data[0]);
		if (node->children[1]->children.size() > 0 && node->children[1]->children[0] != NULL) {
			relate.append(node->children[1]->children[0]->data[0]);
		}

		if (relate.compare("<") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare(">") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl;
		}

		if (relate.compare("=") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;
				
		}

		if (relate.compare("<<") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRZPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare("<>") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare(">>") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl;
		}

		tempCount++;

		//Generate the stmt then nullify the branch
		for (int i = 0; i < node->children[3]->children.size(); ++i) {
			//non-block nodes
			if (node->children[3]->children[i] != NULL && 
				node->children[3]->children[i]->label.compare("block") != 0) 
			{
				generateASM(node->children[3]->children[i]);
				node->children[3]->children[i] = NULL;
				break;
			}
			//block nodes
			else if (node->children[3]->children[i] != NULL) {
				codeGenTraversal(node->children[3]->children[i]);
				node->children[3]->children[i] = NULL;
				break;
			}
		}

		std::cout << "SKIP" << skipCount << ":NOOP" << std::endl;

		skipCount++;
	}

	if (node->label.compare("loop") == 0) {
		//LHS
		getExprString(node->children[0]);
		evaluateExpression();
		lhs = exprString[0];
		exprString.clear();

		//RHS
		getExprString(node->children[2]);
		evaluateExpression();
		rhs = exprString[0];
		exprString.clear();

		//Get relational operator
		std::string relate = "";
		relate.append(node->children[1]->data[0]);
		if (node->children[1]->children.size() > 0 && node->children[1]->children[0] != NULL) {
			relate.append(node->children[1]->children[0]->data[0]);
		}

		//Print loopback label
		std::cout << "LOOP" << loopCount << ":NOOP" << std::endl;

		if (relate.compare("<") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare(">") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl;
		}

		if (relate.compare("=") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;

		}

		if (relate.compare("<<") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRZPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare("<>") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl
				<< "BRPOS SKIP" << skipCount << std::endl;
		}

		if (relate.compare(">>") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRNEG SKIP" << skipCount << std::endl;
		}

		tempCount++;

		//Generate the stmt then nullify the branch
		for (int i = 0; i < node->children[3]->children.size(); ++i) {
			//non-block nodes
			if (node->children[3]->children[i] != NULL &&
				node->children[3]->children[i]->label.compare("block") != 0)
			{
				generateASM(node->children[3]->children[i]);
				node->children[3]->children[i] = NULL;
				break;
			}
			//block nodes
			else if (node->children[3]->children[i] != NULL) {
				codeGenTraversal(node->children[3]->children[i]);
				node->children[3]->children[i] = NULL;
				break;
			}
		}

		std::cout << "BR LOOP" << loopCount << std::endl;

		std::cout << "SKIP" << skipCount << ":NOOP" << std::endl;

		skipCount++;
	}

	if (node->label.compare("assign") == 0) {
		getExprString(node->children[0]);
		evaluateExpression();

		
		if (exprString.size() == 1) {
			std::cout << "LOAD " << exprString[0] << std::endl
				<< "STORE " << node->data[0] << std::endl;
		}
		else {
			std::cout << "LOAD TEMP001" << std::endl
				<< "STORE " << node->data[0] << std::endl;
		}

		

		exprString.clear();
	}	
}

//Function leaves evaluated expression value on the accumulator.
//I absolutely could not get expression parsing to work correctly using the
//tree traversal method, so this is something of a manual override which
//parses the expr left to right, then manually does order of operations using 
//iterators.
void ParseTree::evaluateExpression() {

	if (exprString.size() == 1) {
		if (tokenIsIdentifier(exprString[0])) {
			//std::cout << "expr is identifier " << exprString[0] << std::endl;
		}
		else {
			//std::cout << "expr is integer " << exprString[0] << std::endl;
		}
		return;
	}

	//another manual override
	if (exprString.size() == 3 && tokenIsIdentifier(exprString[0])) {
		std::cout << "LOAD " << exprString[0] << std::endl;

		if (exprString[1].compare("+") == 0) {
			std::cout << "ADD " << exprString[2] << std::endl
				<< "STORE TEMP001" << std::endl;
		}

		if (exprString[1].compare("-") == 0) {
			std::cout << "SUB " << exprString[2] << std::endl
				<< "STORE TEMP001" << std::endl;
		}

		if (exprString[1].compare("*") == 0) {
			std::cout << "MULT " << exprString[2] << std::endl
				<< "STORE TEMP001" << std::endl;
		}

		if (exprString[1].compare("/") == 0) {
			std::cout << "DIV " << exprString[2] << std::endl
				<< "STORE TEMP001" << std::endl;
		}

		return;
	}

	std::stringstream converter;
	bool didCollapse = false;
	bool moreBrackets = false;
	int orderOfOps = 0;
	bool didAll = false;
	bool skipBracketRemoval = false;
	
	//5 indices point to something like this:
	//	[ x + 3 ]	OR	[ x + 3 * 7 ] etc.
	//	^ ^ ^ ^ ^		^     ^ ^ ^ ^
	//
	//the outer iterators hold the range between brackets or entire expr if
	//no brackets.
	//The inner iterators hold a sub expression which gets collapsed leaving the
	//result on the accumulator
	int leftSquare = 0;
	int leftSub = 0;
	int midSub = 0;
	int rightSub = 0;
	int rightSquare = 0;

	int lhs = 0;
	int rhs = 0;
	int result = 0;

	//Scan for a pair of brackets
	bool foundPair = false;
	for (int i = 0; i < exprString.size() && !foundPair; ++i) {
		if (exprString[i].compare("[") == 0) {
			leftSquare = i;
			for (int j = i + 1; j < exprString.size(); ++j) {
				if (exprString[j].compare("[") == 0) {
					break;
				}

				if (exprString[j].compare("]") == 0) {
					rightSquare = j;
					moreBrackets = true;
					break;
				}
			}
		}
	}

	//Set the subexpression iterators inside the square brackets
	if (moreBrackets) {
		leftSub = leftSquare + 1;
		midSub = leftSquare + 2;
		rightSub = leftSquare + 3;
	}
	else {
		skipBracketRemoval = true;
	}
	

	//Collapse all pairs of brackets
	while ((moreBrackets || !didAll) && !skipBracketRemoval) {

		if (midSub >= exprString.size())
			break;

		didCollapse = false;
		moreBrackets = false;

		if ((exprString[midSub].compare("*") == 0 || exprString[midSub].compare("/") == 0) && orderOfOps == 0) {

			//sub expr is multiplication
			if (exprString[midSub].compare("*") == 0) {

				//lhs
				if (tokenIsIdentifier(exprString[leftSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> lhs;
						}
					}
				}
				else {
					converter.str(exprString[leftSub]);
					converter >> lhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//rhs
				if (tokenIsIdentifier(exprString[rightSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> rhs;
						}
					}
				}
				else {
					converter.str(exprString[rightSub]);
					converter >> rhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//Collapse
				result = lhs * rhs;
				converter << result;
				exprString.erase(exprString.begin() + rightSub);
				exprString.erase(exprString.begin() + midSub);
				exprString[leftSub] = converter.str();
				didCollapse = true;

				//Reset converter
				converter.str("");
				converter.clear();
			}

			//sub expr is division
			else if (exprString[midSub].compare("/") == 0) {
				//lhs
				if (tokenIsIdentifier(exprString[leftSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> lhs;
						}
					}
				}
				else {
					converter.str(exprString[leftSub]);
					converter >> lhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//rhs
				if (tokenIsIdentifier(exprString[rightSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> rhs;
						}
					}
				}
				else {
					converter.str(exprString[rightSub]);
					converter >> rhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//Collapse
				result = lhs / rhs;
				converter << result;
				exprString.erase(exprString.begin() + rightSub);
				exprString.erase(exprString.begin() + midSub);
				exprString[leftSub] = converter.str();
				didCollapse = true;

				//Reset converter
				converter.str("");
				converter.clear();
			}
		}

		//Subtraction
		else if (exprString[midSub].compare("-") == 0 && orderOfOps == 1) {
			//lhs
			if (tokenIsIdentifier(exprString[leftSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> lhs;
					}
				}
			}
			else {
				converter.str(exprString[leftSub]);
				converter >> lhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//rhs
			if (tokenIsIdentifier(exprString[rightSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> rhs;
					}
				}
			}
			else {
				converter.str(exprString[rightSub]);
				converter >> rhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//Collapse
			result = lhs - rhs;
			converter << result;
			exprString.erase(exprString.begin() + rightSub);
			exprString.erase(exprString.begin() + midSub);
			exprString[leftSub] = converter.str();
			didCollapse = true;

			//Reset converter
			converter.str("");
			converter.clear();
		}

		//Addition
		else if (exprString[midSub].compare("+") == 0 && orderOfOps == 2) {
			//lhs
			if (tokenIsIdentifier(exprString[leftSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> lhs;
					}
				}
			}
			else {
				converter.str(exprString[leftSub]);
				converter >> lhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//rhs
			if (tokenIsIdentifier(exprString[rightSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> rhs;
					}
				}
			}
			else {
				converter.str(exprString[rightSub]);
				converter >> rhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//Collapse
			result = lhs + rhs;
			converter << result;
			exprString.erase(exprString.begin() + rightSub);
			exprString.erase(exprString.begin() + midSub);
			exprString[leftSub] = converter.str();
			didCollapse = true;

			//Reset converter
			converter.str("");
			converter.clear();
		}

		//-----

		//Rescan for brackets
		if (didCollapse) {
			//Rescan
			foundPair = false;
			for (int i = 0; i < exprString.size() && !foundPair; ++i) {
				if (exprString[i].compare("[") == 0) {
					leftSquare = i;
					for (int j = i + 1; j < exprString.size(); ++j) {
						if (exprString[j].compare("[") == 0) {
							break;
						}

						if (exprString[j].compare("]") == 0) {
							rightSquare = j;
							foundPair = true;
							moreBrackets = true;
							break;
						}
					}
				}
			}

			//This pair of brackets can collapse no further
			if (moreBrackets && rightSquare - leftSquare == 2) {
				exprString.erase(exprString.begin() + rightSquare);
				exprString.erase(exprString.begin() + leftSquare);

				moreBrackets = false;

				foundPair = false;
				for (int i = 0; i < exprString.size() && !foundPair; ++i) {
					if (exprString[i].compare("[") == 0) {
						leftSquare = i;
						for (int j = i + 1; j < exprString.size(); ++j) {
							if (exprString[j].compare("[") == 0) {
								break;
							}

							if (exprString[j].compare("]") == 0) {
								rightSquare = j;
								foundPair = true;
								moreBrackets = true;
								break;
							}
						}
					}
				}
				if (foundPair)
					orderOfOps = 0;
			}
			
			//If a new pair of collapsable brackets is found
			if (moreBrackets && rightSquare - leftSquare > 2) {
				leftSub = leftSquare + 1;
				midSub = leftSquare + 2;
				rightSub = leftSquare + 3;
			}
			
			//There are no more brackets
			else {
				didAll = true;
			}
		}
		else if (exprString[rightSub].compare("]") == 0) {
			foundPair = false;
			for (int i = 0; i < exprString.size() && !foundPair; ++i) {
				if (exprString[i].compare("[") == 0) {
					leftSquare = i;
					for (int j = i + 1; j < exprString.size(); ++j) {
						if (exprString[j].compare("[") == 0) {
							break;
						}

						if (exprString[j].compare("]") == 0) {
							rightSquare = j;
							foundPair = true;
							moreBrackets = true;
							break;
						}
					}
				}
			}

			leftSub = leftSquare + 1;
			midSub = leftSquare + 2;
			rightSub = leftSquare + 3;

			orderOfOps++;
		}
		else {
			//Move iterators
			leftSub++;
			midSub++;
			rightSub++;
		}
	}
	
	//=====

	bool moreSubExpressions = true;
	bool collapsedSubExpression = false;

	leftSub = 0;
	midSub = 1;
	rightSub = 2;

	orderOfOps = 0;

	while (moreSubExpressions) {
		//Collapse all non-bracketed sub expressions
		if ((exprString[midSub].compare("*") == 0 || exprString[midSub].compare("/") == 0) && orderOfOps == 0) {

			//sub expr is multiplication
			if (exprString[midSub].compare("*") == 0) {

				//lhs
				if (tokenIsIdentifier(exprString[leftSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> lhs;
						}
					}
				}
				else {
					converter.str(exprString[leftSub]);
					converter >> lhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//rhs
				if (tokenIsIdentifier(exprString[rightSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> rhs;
						}
					}
				}
				else {
					converter.str(exprString[rightSub]);
					converter >> rhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//Collapse
				result = lhs * rhs;
				converter << result;
				exprString.erase(exprString.begin() + rightSub);
				exprString.erase(exprString.begin() + midSub);
				exprString[leftSub] = converter.str();
				didCollapse = true;

				//Reset converter
				converter.str("");
				converter.clear();
			}

			//sub expr is division
			else if (exprString[midSub].compare("/") == 0) {
				//lhs
				if (tokenIsIdentifier(exprString[leftSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> lhs;
						}
					}
				}
				else {
					converter.str(exprString[leftSub]);
					converter >> lhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//rhs
				if (tokenIsIdentifier(exprString[rightSub])) {
					for (int i = 0; i < symbolTable.size(); ++i) {
						if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
							converter.str(symbolTable[i].second);
							converter >> rhs;
						}
					}
				}
				else {
					converter.str(exprString[rightSub]);
					converter >> rhs;
				}

				//Reset converter
				converter.str("");
				converter.clear();

				//Collapse
				result = lhs / rhs;
				converter << result;
				exprString.erase(exprString.begin() + rightSub);
				exprString.erase(exprString.begin() + midSub);
				exprString[leftSub] = converter.str();
				didCollapse = true;

				//Reset converter
				converter.str("");
				converter.clear();
			}
		}

		//Subtraction
		else if (exprString[midSub].compare("-") == 0 && orderOfOps == 1) {
			//lhs
			if (tokenIsIdentifier(exprString[leftSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> lhs;
					}
				}
			}
			else {
				converter.str(exprString[leftSub]);
				converter >> lhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//rhs
			if (tokenIsIdentifier(exprString[rightSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> rhs;
					}
				}
			}
			else {
				converter.str(exprString[rightSub]);
				converter >> rhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//Collapse
			result = lhs - rhs;
			converter << result;
			exprString.erase(exprString.begin() + rightSub);
			exprString.erase(exprString.begin() + midSub);
			exprString[leftSub] = converter.str();
			didCollapse = true;

			//Reset converter
			converter.str("");
			converter.clear();
		}

		//Addition
		else if (exprString[midSub].compare("+") == 0 && orderOfOps == 2) {
			//lhs
			if (tokenIsIdentifier(exprString[leftSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[leftSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> lhs;
					}
				}
			}
			else {
				converter.str(exprString[leftSub]);
				converter >> lhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//rhs
			if (tokenIsIdentifier(exprString[rightSub])) {
				for (int i = 0; i < symbolTable.size(); ++i) {
					if (symbolTable[i].first.compare(exprString[rightSub]) == 0) {
						converter.str(symbolTable[i].second);
						converter >> rhs;
					}
				}
			}
			else {
				converter.str(exprString[rightSub]);
				converter >> rhs;
			}

			//Reset converter
			converter.str("");
			converter.clear();

			//Collapse
			result = lhs + rhs;
			converter << result;
			exprString.erase(exprString.begin() + rightSub);
			exprString.erase(exprString.begin() + midSub);
			exprString[leftSub] = converter.str();
			didCollapse = true;

			//Reset converter
			converter.str("");
			converter.clear();
		}

		if (exprString.size() == 1) {
			moreSubExpressions = false;
		}

		if (rightSub >= exprString.size()) {
			leftSub = 0;
			midSub = 1;
			rightSub = 2;

			if(orderOfOps < 2)
				orderOfOps++;
			else
				orderOfOps = 0;
			continue;
		}

		leftSub++;
		midSub++;
		rightSub++;
	}
	
	//std::cout << "evaluated expr = " << exprString[0] << std::endl;

	return;
}

bool ParseTree::tokenIsIdentifier(std::string token) {
	//first char is a-z
	if (token[0] >= 97 && token[0] <= 122)
		return true;
	return false;
}

void ParseTree::getExprString(node* exprNode) {
	if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("identifier") == 0) {
		exprString.push_back(exprNode->data[1]);
	}
	else if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("integer") == 0) {
		exprString.push_back(exprNode->data[1]);
	}
	else if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("[") == 0) {
		exprString.push_back(exprNode->data[0]);
	}
	if (exprNode->label.compare("z") == 0) {
		exprString.push_back(exprNode->data[0]);
	}
		
	if (exprNode->label.compare("x") == 0) {
		exprString.push_back(exprNode->data[0]);
	}
		
	if (exprNode->label.compare("y") == 0) {
		exprString.push_back(exprNode->data[0]);
	}
		
	//Recurse
	for (int i = 0; i < exprNode->children.size(); ++i) {
		if (exprNode->children[i] != NULL) {
			getExprString(exprNode->children[i]);
		}
	}
		
	if (exprNode->label.compare("r") == 0 && exprNode->data[1].compare("]") == 0) {
		exprString.push_back(exprNode->data[1]);
	}
		
	return;
}

//int ParseTree::evaluateExpression(node* exprNode) {
//	static int val = 0;
//	std::stringstream converter;
//	std::stringstream converter2;
//	std::stringstream result;
//	static int stackLevel = 0;
//	
//	int lhs, rhs;
//
//	//std::cout << exprNode->label << std::endl;
//
//	if (exprNode->label.compare("r") == 0) {
//		if (exprNode->data[0].compare("integer") == 0) {
//			//std::cout << "pushing " << exprNode->data[1] << std::endl;
//			exprStack[stackLevel].push_back(exprNode->data[1]);
//			converter.str(exprNode->data[1]);
//			converter >> val;
//		}
//		else if (exprNode->data[0].compare("identifier") == 0) {
//			for (int i = 0; i < symbolTable.size(); ++i) {
//				if (symbolTable[i].first.compare(exprNode->data[1]) == 0) {
//					//std::cout << "pushing " << symbolTable[i].second << std::endl;
//					exprStack[stackLevel].push_back(symbolTable[i].second);
//					converter.str(symbolTable[i].second);
//					converter >> val;
//				}
//			}
//		}
//	}
//	else if (exprNode->label.compare("z") == 0) {
//		//std::cout << "pushing +" << std::endl;
//		exprStack[stackLevel].push_back("+");
//
//		exprStack.resize(exprStack.size() + 1);
//		stackLevel++;
//
//		evaluateExpression(exprNode->children[0]);
//
//		if (exprStack[stackLevel].size() == 1 && stackLevel > 0) {
//			exprStack[stackLevel - 1].push_back(exprStack[stackLevel][0]);
//			stackLevel--;
//			exprStack.pop_back();
//		}
//	}
//	else if (exprNode->label.compare("y") == 0) {
//		//std::cout << "pushing -" << std::endl;
//		exprStack[stackLevel].push_back("-");
//
//		evaluateExpression(exprNode->children[0]);
//
//		/*exprStack.resize(exprStack.size() + 1);
//		stackLevel++;*/
//
//	}
//	else if (exprNode->label.compare("x") == 0) {
//		//std::cout << "pushing " << exprNode->data[0] << std::endl;
//		exprStack[stackLevel].push_back(exprNode->data[0]);
//
//		evaluateExpression(exprNode->children[0]);
//	}
//	else if (exprNode->label.compare("expr") == 0) {
//		if (exprNode->children[0] != NULL) {
//			evaluateExpression(exprNode->children[0]);
//		}
//
//		if (exprNode->children[1] != NULL) {
//			evaluateExpression(exprNode->children[1]);
//		}
//	}
//	else {
//		//Right to left traversal
//		for (int i = 0; i < exprNode->children.size(); ++i) {
//			if (exprNode->children[i] != NULL) {
//				evaluateExpression(exprNode->children[i]);
//			}
//		}
//	}
//
//	//Evaluate a sub expr
//	if (exprStack[stackLevel].size() == 3) {
//		converter.str(exprStack[stackLevel].front());
//		converter >> lhs;
//		converter2.str(exprStack[stackLevel].back());
//		converter2 >> rhs;
//
//		if (exprStack[stackLevel][1].compare("/") == 0) {
//			val = lhs / rhs;
//			//std::cout << lhs << "/" << rhs << "=" << val << std::endl;
//		}
//
//		if (exprStack[stackLevel][1].compare("*") == 0) {
//			val = lhs * rhs;
//			//std::cout << lhs << "*" << rhs << "=" << val << std::endl;
//		}
//
//		if (exprStack[stackLevel][1].compare("+") == 0) {
//			val = lhs + rhs;
//			//std::cout << lhs << "+" << rhs << "=" << val << std::endl;
//		}
//
//		if (exprStack[stackLevel][1].compare("-") == 0) {
//			val = lhs - rhs;
//			//std::cout << lhs << "-" << rhs << "=" << val << std::endl;
//		}
//
//		if (stackLevel > 0) {
//			exprStack.pop_back();
//			stackLevel--;
//			result << val;
//			exprStack[stackLevel].push_back(result.str());
//		}
//		else {
//			exprStack[stackLevel].clear();
//			result << val;
//			exprStack[stackLevel].push_back(result.str());
//		}
//		
//	}
//
//	return val;
//}

void ParseTree::printUsedVars() {
	std::cout << "STOP" << std::endl;

	for (int i = 0; i < tempCount; ++i) {
		std::cout << "TEMP" << i << " 0" << std::endl;
	}

	for (int i = 0; i < symbolTable.size(); ++i) {
		std::cout << symbolTable[i].first << " " << symbolTable[i].second << std::endl;
	}

	std::cout << "TEMP001 0" << std::endl;
}

void ParseTree::getOutFileName(std::string name) {
	this->outFileName = name;
}