//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include "Token.h"

//Static init
bool Token::isInitialized = false;
std::vector<std::string> Token::keywords;

//Constructor
Token::Token(TokenID id, std::string instance, int lineNum) 
    : id(id), instance(instance), lineNum(lineNum)
{
    if(!isInitialized) {
        keywords.push_back("start");
        keywords.push_back("stop");
        keywords.push_back("iterate");
        keywords.push_back("void");
        keywords.push_back("var");
        keywords.push_back("return");
        keywords.push_back("in");
        keywords.push_back("out");
        keywords.push_back("program");
        keywords.push_back("cond");
        keywords.push_back("then");
        keywords.push_back("let");

        isInitialized = true;
    }
}

//Accessors
TokenID Token::getID() { return id; }
int Token::getLineNum() { return lineNum; }
std::string Token::getInstance() { return instance; }

std::string Token::idName(int id) {
    switch(id) {
        case INTEGER_tk: return "INTEGER_tk";
        case IDENTIFIER_tk: return "IDENTIFIER_tk";
        case EOF_tk: return "EOF_tk";
        case OPERATOR_tk: return "OPERATOR_tk";
        case KWD_START_tk: return "KWD_START_tk";
        case KWD_STOP_tk: return "KWD_STOP_tk";
        case KWD_ITERATE_tk: return "KWD_ITERATE_tk";
        case KWD_VOID_tk: return "KWD_VOID_tk";
        case KWD_VAR_tk: return "KWD_VAR_tk";
        case KWD_RETURN_tk: return "KWD_RETURN_tk";
        case KWD_IN_tk: return "KWD_IN_tk";
        case KWD_OUT_tk: return "KWD_OUT_tk";
        case KWD_PROGRAM_tk: return "KWD_PROGRAM_tk";
        case KWD_COND_tk: return "KWD_COND_tk";
        case KWD_THEN_tk: return "KWD_THEN_tk";
        case KWD_LET_tk: return "KWD_LET_tk";
        
        default:
            std::cerr << "ERROR: Invalid selection made in function idName(int id)" << std::endl;
            return "INVALID_TOKEN_ID";
    }
}

//Debug
void Token::printToken() {

    int columnSpacing = 35;

    std::stringstream ss;
    std::string idString;
    std::string instanceString;
    std::string lineNumString;

    //Create idString
    ss << "ID { " << std::setw(2) << std::setfill('0') << id << ", " << idName(id) << " }";
    idString = ss.str();

    ss.str(""); //Clear ss buffer

    //Create instanceString
    ss << "INSTANCE { \"" << instance << "\" }";
    instanceString = ss.str();

    ss.str(""); //Clear ss buffer

    //Create lineNumString
    ss << "LINE# " << lineNum;
    lineNumString = ss.str();

    //Print token
    std::cout << std::left << std::setw(columnSpacing) << idString;
    std::cout << std::left << std::setw(columnSpacing) << instanceString;
    std::cout << std::left << std::setw(columnSpacing) << lineNumString;
    std::cout << std::endl;
}

void Token::printKeywordVector() {
    for(int i = 0; i < keywords.size(); ++i) {
        std::cout << keywords.at(i) << " ";
    }
    std::cout << std::endl;

    return;
}


std::string Token::findKeyword(std::string checkString) {
    for(int i = 0; i < keywords.size(); ++i) {
        if(checkString.compare(keywords[i]) == 0)
            return keywords[i];
    }

    return "NO_MATCH";
}