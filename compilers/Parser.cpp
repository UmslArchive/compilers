//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "Parser.h"

//Constructor
Parser::Parser() :
    currentToken(Token(ERROR_tk, "PARSE_BEGIN", 0))
{}

//Init
void Parser::sendFileDataToScanner(std::string fileData) {
    scanner.loadFileData(fileData);
}

ParseTree Parser::parse() {
    parseTree.root = program();

    /* std::cout << std::endl << "PARSE TREE:" << std::endl;

    parseTree.printAll(parseTree.root); */

    return parseTree;
}

node* Parser::program() {
    if(debug) std::cout << "entered PROGRAM" << std::endl;

    //std::cout << std::endl << "PARSING..." << std::endl << std::endl;

    node* programNode = new node("program");
    currentToken = scanner.getNextToken();
    //currentToken.printToken();
    programNode->children.push_back(vars());
    programNode->children.push_back(block());

    if(currentToken.getID() == EOF_tk) {
        //std::cout << "\nFINISHED -- VALID PROGRAM" << std::endl;
        return programNode;
    }
    else {
        error();
    }

    return NULL; 
}

node* Parser::block() {
    if(debug) std::cout << "entered BLOCK" << std::endl;
    node* blockNode = new node("block");
    if(currentToken.getID() == KWD_START_tk){
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        blockNode->children.push_back(vars());
        blockNode->children.push_back(stats());
        if(currentToken.getID() == KWD_STOP_tk) {
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
			blockNode->children.push_back(new node("endBlock"));
            if(debug) std::cout << "exit BLOCK 1" << std::endl;
            return blockNode;
        }
        else {
            error();
        }
    }
    
    if(debug) std::cout << "exit BLOCK 2" << std::endl;
    return NULL;
}

node* Parser::vars() {
    if(debug) std::cout << "entered VARS" << std::endl;
    node* varsNode = new node("vars");
    if(currentToken.getID() == KWD_VAR_tk) {
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(currentToken.getID() == IDENTIFIER_tk) {
            varsNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(":") == 0) {
                currentToken = scanner.getNextToken();
                //currentToken.printToken();
                if(currentToken.getID() == INTEGER_tk) {
                    varsNode->data.push_back(currentToken.getInstance());
                    currentToken = scanner.getNextToken();
                    //currentToken.printToken();
                    varsNode->children.push_back(vars());
                    if(debug) std::cout << "exit VARS 1" << std::endl;
                    return varsNode;
                }
                else {
                    error();
                }
            }
            else {
                error();
            }
        }
        else {
            error();
        }
    }

    if(debug) std::cout << "exit VARS 2" << std::endl;
    return NULL;
}

node* Parser::expr() {
    if(debug) std::cout << "entered EXPR" << std::endl;
    node* exprNode = new node("expr"); 
    exprNode->children.push_back(A());
    exprNode->children.push_back(Z());
    if(debug) std::cout << "exit EXPR" << std::endl;

    return exprNode;
}
node* Parser::Z() {
    if(debug) std::cout << "entered Z" << std::endl;
    node* zNode = new node("z");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("+") == 0) {
        zNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        zNode->children.push_back(expr());
        if(debug) std::cout << "exit Z" << std::endl;
        return zNode;
    }
    return NULL;
}

node* Parser::A() {
    if(debug) std::cout << "entered A" << std::endl;
    node* aNode = new node("a");
    aNode->children.push_back(N());
    aNode->children.push_back(Y());
    if(debug) std::cout << "exit A" << std::endl;

    return aNode;
}

node* Parser::Y() {
    if(debug) std::cout << "entered Y" << std::endl;
    node* yNode = new node("y");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("-") == 0) {
        yNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        yNode->children.push_back(A());
		if (debug) std::cout << "exit Y" << std::endl;
		return yNode;
    }
    if(debug) std::cout << "exit Y NULL" << std::endl;
    return NULL;
}

node* Parser::N() {
    if(debug) std::cout << "entered N" << std::endl;
    node* nNode = new node("n");
    nNode->children.push_back(M());
    nNode->children.push_back(X());
    if(debug) std::cout << "exit N" << std::endl;

    return nNode;
}

node* Parser::X() {
    if(debug) std::cout << "entered X" << std::endl;
    node* xNode = new node("x");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("/") == 0) {
        xNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        xNode->children.push_back(N());
        if(debug) std::cout << "exit X" << std::endl;
        return xNode;
    }
    else if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("*") == 0) {
        xNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        xNode->children.push_back(N());
        if(debug) std::cout << "exit X" << std::endl;
        return xNode;
    }

    return NULL;
}

node* Parser::M() {
    if(debug) std::cout << "entered M" << std::endl;
    node* mNode = new node("m");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("-") == 0) {
        mNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        mNode->children.push_back(M());
        if(debug) std::cout << "exit M" << std::endl;
        return mNode;
    }
    else {
        mNode->children.push_back(R());
        if(debug) std::cout << "exit M" << std::endl;
        return mNode;
    }
}

node* Parser::R() {
    if(debug) std::cout << "entered R" << std::endl;
    node* rNode = new node("r");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("[") == 0) {
        rNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        rNode->children.push_back(expr());
        if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("]") == 0) {
            rNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(debug) std::cout << "exit R" << std::endl;
            return rNode;
        }
        else {
            error();
        }
            
    }
    else if(currentToken.getID() == IDENTIFIER_tk) {
		rNode->data.push_back("identifier");
        rNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit R" << std::endl;
        return rNode;
    }
    else if(currentToken.getID() == INTEGER_tk) {
        rNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit R" << std::endl;
        return rNode;
    }
    else {
        error();
    }

    return NULL;
}

node* Parser::stats() {
    if(debug) std::cout << "entered STATS" << std::endl;
    node* statsNode = new node("stats");
    statsNode->children.push_back(stat());
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(";") == 0) {
        //statsNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        statsNode->children.push_back(mStat());
        if(debug) std::cout << "exit STATS" << std::endl;
        return statsNode;
    }
    else {
        error();
    }

    return NULL;
}

node* Parser::mStat() {
    if(debug) std::cout << "entered MSTAT" << std::endl;
    node* mStatNode = new node("mstat");
    mStatNode->children.push_back(stat());
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(";") == 0) {
        //mStatNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        mStatNode->children.push_back(mStat());
        if(debug) std::cout << "exit MSTAT 1" << std::endl;
        return mStatNode;
    }

    if(debug) std::cout << "exit MSTAT 2" << std::endl;
    return NULL;
}

node* Parser::stat() {
    if(debug) std::cout << "entered STAT" << std::endl;
    node* statNode = new node("stat");
    statNode->children.push_back(in());
    statNode->children.push_back(out());
    statNode->children.push_back(block());
    statNode->children.push_back(IF());
    statNode->children.push_back(loop());
    statNode->children.push_back(assign());

    if(debug) std::cout << "exit STAT" << std::endl;
    return statNode;
}

node* Parser::in() {
    if(debug) std::cout << "entered IN" << std::endl;
    node* inNode = new node("in");
    if(currentToken.getID() == KWD_IN_tk) {
        inNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(currentToken.getID() == IDENTIFIER_tk) {
            inNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(debug) std::cout << "exit IN" << std::endl;
            return inNode;
        }
        else {
            error();
        }
    }
    return NULL;
}

node* Parser::out() {
    if(debug) std::cout << "entered OUT" << std::endl;
    node* outNode = new node("out");
    if(currentToken.getID() == KWD_OUT_tk) {
        outNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        outNode->children.push_back(expr());
        if(debug) std::cout << "exit OUT" << std::endl;
        return outNode;
    }

    return NULL;
}

node* Parser::IF() {
    if(debug) std::cout << "entered IF" << std::endl;
    node* ifNode = new node("if");
    if(currentToken.getID() == KWD_COND_tk) {
        ifNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("(") == 0) {
            ifNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("(") == 0) {
                ifNode->data.push_back(currentToken.getInstance());
                currentToken = scanner.getNextToken();
                //currentToken.printToken();
                ifNode->children.push_back(expr());
                ifNode->children.push_back(RO());
                ifNode->children.push_back(expr());
                if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(")") == 0) {
                    ifNode->data.push_back(currentToken.getInstance());
                    currentToken = scanner.getNextToken();
                    //currentToken.printToken();
                    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(")") == 0) {
                        ifNode->data.push_back(currentToken.getInstance());
                        currentToken = scanner.getNextToken();
                        //currentToken.printToken();
                        ifNode->children.push_back(stat());
                        if(debug) std::cout << "exit IF" << std::endl;
                        return ifNode;
                    }
                    else {
                        error();
                    }
                }
                else {
                    error();
                }
            }
            else {
                error();
            }
        }
        else {
            error();
        }
    }

    if(debug) std::cout << "exit IF" << std::endl;
    return NULL;
}

node* Parser::loop() {
    if(debug) std::cout << "entered LOOP" << std::endl;
    node* loopNode = new node("loop");
    if(currentToken.getID() == KWD_ITERATE_tk) {
        loopNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("(") == 0) {
            loopNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("(") == 0) {
                loopNode->data.push_back(currentToken.getInstance());
                currentToken = scanner.getNextToken();
                //currentToken.printToken();
                loopNode->children.push_back(expr());
                loopNode->children.push_back(RO());
                loopNode->children.push_back(expr());
                if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(")") == 0) {
                    loopNode->data.push_back(currentToken.getInstance());
                    currentToken = scanner.getNextToken();
                    //currentToken.printToken();
                    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(")") == 0) {
                        loopNode->data.push_back(currentToken.getInstance());
                        currentToken = scanner.getNextToken();
                        //currentToken.printToken();
                        loopNode->children.push_back(stat());
                        if(debug) std::cout << "exit LOOP" << std::endl;
                        return loopNode;
                    }
                    else {
                        error();
                    }
                }
                else {
                    error();
                }
            }
            else {
                error();
            }
        }
        else {
            error();
        }
    }
    if(debug) std::cout << "exit LOOP" << std::endl;
    return NULL;
}

node* Parser::assign() {
    if(debug) std::cout << "entered ASSIGN" << std::endl;
    node* assignNode = new node("assign");
    if(currentToken.getID() == IDENTIFIER_tk) {
        assignNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("<") == 0) {
            assignNode->data.push_back(currentToken.getInstance());
            currentToken = scanner.getNextToken();
            //currentToken.printToken();
            if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("<") == 0) {
                assignNode->data.push_back(currentToken.getInstance());
                currentToken = scanner.getNextToken();
                //currentToken.printToken();
                assignNode->children.push_back(expr());
                if(debug) std::cout << "exit ASSIGN 1" << std::endl;
                return assignNode;
            }
            else {
                error();
            }
        }
        else {
            error();
        }
    }
    if(debug) std::cout << "exit ASSIGN 2" << std::endl;
    return NULL;
}

node* Parser::RO() {
    if(debug) std::cout << "entered RO" << std::endl;
    node* roNode = new node("ro");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("<") == 0) {
        roNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        roNode->children.push_back(W());
        if(debug) std::cout << "exit RO" << std::endl;
        return roNode;
    }
    else if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(">") == 0) {
        roNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        roNode->children.push_back(T());
        if(debug) std::cout << "exit RO" << std::endl;
        return roNode;
    }
    else if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("=") == 0) {
        roNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit RO" << std::endl;
        return roNode;
    }
    else {
        error();
    }

    return NULL;
}

node* Parser::W() {
    if(debug) std::cout << "entered W" << std::endl;
    node* wNode = new node("w");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare("<") == 0) {
        wNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit W" << std::endl;
        return wNode;
    }
    else if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(">") == 0) {
        wNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit W" << std::endl;
        return wNode;
    }

    if(debug) std::cout << "exit W" << std::endl;
    return NULL;
}

node* Parser::T() {
    if(debug) std::cout << "entered T" << std::endl;
    node* tNode = new node("t");
    if(currentToken.getID() == OPERATOR_tk && currentToken.getInstance().compare(">") == 0) {
        tNode->data.push_back(currentToken.getInstance());
        currentToken = scanner.getNextToken();
        //currentToken.printToken();
        if(debug) std::cout << "exit T" << std::endl;
        return tNode;
    }

    if(debug) std::cout << "exit T" << std::endl;
    return NULL;
}

void Parser::error() {
    std::cerr << "\nPARSE ERROR --- INVALID PROGRAM" << std::endl << std::endl;
    std::cout << "Invalid token found:" << std::endl;
    currentToken.printToken();

    std::cout << std::endl;
    exit(1);
}