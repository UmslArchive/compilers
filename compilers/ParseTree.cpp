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
	tempCount(0)
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
	int exprVal;
	int lhs, rhs;
	std::stringstream converter;

	if (node->label.compare("out") == 0) {
		exprVal = evaluateExpression(node->children[0]);
		std::cout << "WRITE " << exprVal << std::endl;
	}

	if (node->label.compare("in") == 0) {
		std::cout << "READ " << node->data[1];
	}

	if (node->label.compare("if") == 0) {
		//Evaluate expressions
		lhs = evaluateExpression(node->children[0]);
		rhs = evaluateExpression(node->children[2]);
		
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
				<< "BRZERO SKIP" << skipCount << std::endl;
		}

		if (relate.compare(">>") == 0) {
			std::cout << "LOAD " << rhs << std::endl
				<< "STORE TEMP" << tempCount << std::endl
				<< "LOAD " << lhs << std::endl
				<< "SUB TEMP" << tempCount << std::endl
				<< "BRZNEG SKIP" << skipCount << std::endl;
		}

		tempCount++;

		//Generate the stmt
		for (int i = 0; i < node->children[3]->children.size(); ++i) {
			if (node->children[3]->children[i] != NULL) {
				generateASM(node->children[3]->children[i]);
				node->children[3]->children[i] = NULL;
				break;
			}
		}

		std::cout << "SKIP" << skipCount << ":NOOP" << std::endl;

		skipCount++;

	}

	if (node->label.compare("assign") == 0) {
		exprVal = evaluateExpression(node->children[0]);
		std::cout << "LOAD " << exprVal << std::endl
			<< "STORE " << node->data[0] << std::endl;

		//Update symbol table
		for (int i = 0; i < symbolTable.size(); ++i) {
			if (symbolTable[i].first.compare(node->data[0]) == 0) {
				converter << exprVal;
				converter << symbolTable[i].second;
			}
		}
	}

	if (node->label.compare("loop") == 0) {

	}
	
}

int ParseTree::evaluateExpression(node* exprNode) {
	static int val = 0;
	std::stringstream converter;
	std::stringstream converter2;
	std::stringstream result;
	static int stackLevel = 0;
	
	int lhs, rhs;

	//std::cout << exprNode->label << std::endl;

	if (exprNode->label.compare("r") == 0) {
		if (exprNode->data[0].compare("integer") == 0) {
			//std::cout << "pushing " << exprNode->data[1] << std::endl;
			exprStack[stackLevel].push_back(exprNode->data[1]);
			converter.str(exprNode->data[1]);
			converter >> val;
		}
		else if (exprNode->data[0].compare("identifier") == 0) {
			for (int i = 0; i < symbolTable.size(); ++i) {
				if (symbolTable[i].first.compare(exprNode->data[1]) == 0) {
					//std::cout << "pushing " << symbolTable[i].second << std::endl;
					exprStack[stackLevel].push_back(symbolTable[i].second);
					converter.str(symbolTable[i].second);
					converter >> val;
				}
			}
		}
	}
	else if (exprNode->label.compare("z") == 0) {
		//std::cout << "pushing +" << std::endl;
		exprStack[stackLevel].push_back("+");

		exprStack.resize(exprStack.size() + 1);
		stackLevel++;

		evaluateExpression(exprNode->children[0]);

		if (exprStack[stackLevel].size() == 1 && stackLevel > 0) {
			exprStack[stackLevel - 1].push_back(exprStack[stackLevel][0]);
			stackLevel--;
			exprStack.pop_back();
		}
	}
	else if (exprNode->label.compare("y") == 0) {
		//std::cout << "pushing -" << std::endl;
		exprStack[stackLevel].push_back("-");

		evaluateExpression(exprNode->children[0]);

		/*exprStack.resize(exprStack.size() + 1);
		stackLevel++;*/

	}
	else if (exprNode->label.compare("x") == 0) {
		//std::cout << "pushing " << exprNode->data[0] << std::endl;
		exprStack[stackLevel].push_back(exprNode->data[0]);

		evaluateExpression(exprNode->children[0]);
	}
	else if (exprNode->label.compare("expr") == 0) {
		if (exprNode->children[0] != NULL) {
			evaluateExpression(exprNode->children[0]);
		}

		if (exprNode->children[1] != NULL) {
			evaluateExpression(exprNode->children[1]);
		}
	}
	else {
		//Right to left traversal
		for (int i = 0; i < exprNode->children.size(); ++i) {
			if (exprNode->children[i] != NULL) {
				evaluateExpression(exprNode->children[i]);
			}
		}
	}

	//Evaluate a sub expr
	if (exprStack[stackLevel].size() == 3) {
		converter.str(exprStack[stackLevel].front());
		converter >> lhs;
		converter2.str(exprStack[stackLevel].back());
		converter2 >> rhs;

		if (exprStack[stackLevel][1].compare("/") == 0) {
			val = lhs / rhs;
			//std::cout << lhs << "/" << rhs << "=" << val << std::endl;
		}

		if (exprStack[stackLevel][1].compare("*") == 0) {
			val = lhs * rhs;
			//std::cout << lhs << "*" << rhs << "=" << val << std::endl;
		}

		if (exprStack[stackLevel][1].compare("+") == 0) {
			val = lhs + rhs;
			//std::cout << lhs << "+" << rhs << "=" << val << std::endl;
		}

		if (exprStack[stackLevel][1].compare("-") == 0) {
			val = lhs - rhs;
			//std::cout << lhs << "-" << rhs << "=" << val << std::endl;
		}

		if (stackLevel > 0) {
			exprStack.pop_back();
			stackLevel--;
			result << val;
			exprStack[stackLevel].push_back(result.str());
		}
		else {
			exprStack[stackLevel].clear();
			result << val;
			exprStack[stackLevel].push_back(result.str());
		}
		
	}

	return val;
}

void ParseTree::printUsedVars() {
	std::cout << "STOP" << std::endl;

	for (int i = 0; i < tempCount; ++i) {
		std::cout << "TEMP" << i << " 0" << std::endl;
	}

	for (int i = 0; i < symbolTable.size(); ++i) {
		std::cout << symbolTable[i].first << " " << symbolTable[i].second << std::endl;
	}
}

//void ParseTree::evaluateExpression(node* exprNode, std::vector<std::string>& exprString) {
//	if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("identifier") == 0) {
//		exprString.push_back(exprNode->data[1]);
//	}
//	else if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("integer") == 0) {
//		exprString.push_back(exprNode->data[1]);
//	}
//	else if (exprNode->label.compare("r") == 0 && exprNode->data[0].compare("[") == 0) {
//		exprString.push_back(exprNode->data[0]);
//	}
//	if (exprNode->label.compare("z") == 0) {
//		exprString.push_back(exprNode->data[0]);
//	}
//
//	if (exprNode->label.compare("x") == 0) {
//		exprString.push_back(exprNode->data[0]);
//	}
//
//	if (exprNode->label.compare("y") == 0) {
//		exprString.push_back(exprNode->data[0]);
//	}
//
//	//Recurse
//	for (int i = 0; i < exprNode->children.size(); ++i) {
//		if (exprNode->children[i] != NULL) {
//			evaluateExpression(exprNode->children[i], exprString);
//		}
//	}
//
//	if (exprNode->label.compare("r") == 0 && exprNode->data[1].compare("]") == 0) {
//		exprString.push_back(exprNode->data[1]);
//	}
//
//	return;
//}
//
//void ParseTree::manualOverride(std::vector<std::string>& exprResult) {
//	int lhs, rhs;
//	std::vector<int> collapsePositions;
//	std::string converted;
//	std::ostringstream converter;
//	std::vector<std::string> opOrder;
//
//	std::vector<std::string> subString;
//
//
//	//Handle brackets
//	bool collapsable = true;
//	bool bracketCollapsable = true;
//	bool foundBracket = false;
//	bool didMultDiv = false;
//	bool didSub = false;
//	bool didAdd = false;
//	while (collapsable) {
//		foundBracket = false;
//		for (int i = 0; i < exprResult.size(); ++i) {
//			if (exprResult[i].compare("[") == 0) {
//				foundBracket = true;
//				//save position of most recent left bracket
//				if (collapsePositions.size() > 0)
//					collapsePositions.pop_back();
//
//				collapsePositions.push_back(i);
//			}
//
//			if (exprResult[i].compare("]") == 0) {
//				//save position of right brace of inner most bracket
//				collapsePositions.push_back(i);
//
//				//Create substring from stuff in inner brackets
//				for (int i = collapsePositions[0]; i < collapsePositions[1] + 1; ++i) {
//					subString.push_back(exprResult[i]);
//				}
//
//				//Collapse substring
//				bracketCollapsable = true;
//				while (bracketCollapsable) {
//					lhs = 0;
//					rhs = 0;
//					converter.str("");
//					didMultDiv = false;
//					didSub = false;
//					didAdd = false;
//
//					//Multiplication and division
//					for (int i = 0; i < subString.size(); ++i) {
//						if (subString[i].compare("*") == 0 || subString[i].compare("/") == 0) {
//							if (subString[i].compare("*") == 0) {
//								if (isalpha(subString[i - 1][0]) || isalpha(subString[i + 1][0])) {
//									subString[i] = "&";
//									break;
//								}
//								std::istringstream(subString[i - 1]) >> lhs;
//								std::istringstream(subString[i + 1]) >> rhs;
//								converter << lhs * rhs;
//								converted = converter.str();
//								subString.insert(subString.begin() + i + 2, converted);
//								subString.erase(subString.begin() + i - 1, subString.begin() + i + 2);
//								didMultDiv = true;
//								break;
//							}
//							if (subString[i].compare("/") == 0) {
//								if (isalpha(subString[i - 1][0]) || isalpha(subString[i + 1][0])) {
//									subString[i] = "?";
//									break;
//								}
//								std::istringstream(subString[i - 1]) >> lhs;
//								std::istringstream(subString[i + 1]) >> rhs;
//								converter << lhs / rhs;
//								converted = converter.str();
//								subString.insert(subString.begin() + i + 2, converted);
//								subString.erase(subString.begin() + i - 1, subString.begin() + i + 2);
//								didMultDiv = true;
//								break;
//							}
//						}
//					}
//
//					//Subtraction
//					for (int i = 0; i < subString.size(); ++i) {
//						if (subString[i].compare("-") == 0 && !didMultDiv) {
//							if (isalpha(subString[i - 1][0]) || isalpha(subString[i + 1][0])) {
//								subString[i] = "_";
//								break;
//							}
//							std::istringstream(subString[i - 1]) >> lhs;
//							std::istringstream(subString[i + 1]) >> rhs;
//							converter << lhs - rhs;
//							converted = converter.str();
//							subString.insert(subString.begin() + i + 2, converted);
//							subString.erase(subString.begin() + i - 1, subString.begin() + i + 2);
//							didSub = true;
//							break;
//						}
//					}
//
//					//Addition
//					for (int i = 0; i < subString.size(); ++i) {
//						if (subString[i].compare("+") == 0 && !didSub && !didMultDiv) {
//							//skip operator
//							if (isalpha(subString[i - 1][0])) {
//								subString[i] = "=";
//								break;
//							}
//
//							//Skip operator
//							if (isalpha(subString[i + 1][0])) {
//								subString[i] = "=";
//								break;
//							}
//							std::istringstream(subString[i - 1]) >> lhs;
//							std::istringstream(subString[i + 1]) >> rhs;
//							converter << lhs + rhs;
//							converted = converter.str();
//							subString.insert(subString.begin() + i + 2, converted);
//							subString.erase(subString.begin() + i - 1, subString.begin() + i + 2);
//							didAdd = true;
//							break;
//						}
//					}
//
//					//Scan for any remaining operators
//					bool operatorsRemain = false;
//					for (int i = 0; i < subString.size(); ++i) {
//						if (subString[i].compare("*") == 0 ||
//							subString[i].compare("-") == 0 ||
//							subString[i].compare("/") == 0 ||
//							subString[i].compare("+") == 0)
//						{
//							operatorsRemain = true;
//						}
//					}
//
//					if (!operatorsRemain) {
//						bracketCollapsable = false;
//					}
//				}
//
//				//Replace operator fillers in substring
//				for (int i = 0; i < subString.size(); ++i) {
//					if (subString[i].compare("=") == 0) {
//						subString[i] = "+";
//					}
//					if (subString[i].compare("_") == 0) {
//						subString[i] = "-";
//					}
//					if (subString[i].compare("?") == 0) {
//						subString[i] = "/";
//					}
//					if (subString[i].compare("&") == 0) {
//						subString[i] = "*";
//					}
//				}
//
//				//Replace bracketed expression with substring
//				for (int i = 1; i < subString.size() - 1; ++i) {
//					exprResult.insert(exprResult.begin() + collapsePositions[1] + i, subString[i]);
//				}
//				exprResult.erase(exprResult.begin() + collapsePositions[0], exprResult.begin() + collapsePositions[1] + 1);
//				collapsePositions.clear();
//
//				//break the exprResult for loop
//				break;
//			}
//		}
//
//		for (int i = 0; i < subString.size(); ++i) {
//			std::cout << subString[i] << " ";
//		}
//		std::cout << std::endl;
//
//		subString.clear();
//
//		for (int i = 0; i < exprResult.size(); ++i) {
//			std::cout << exprResult[i] << " ";
//		}
//		std::cout << std::endl;
//
//		if (!foundBracket) {
//			collapsable = false;
//		}
//	}
//
//}