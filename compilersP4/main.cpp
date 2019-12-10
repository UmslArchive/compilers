//Author:   Colby Ackerman
//Class:    CS4280 Program Translations
//Assign:   Project 2
//Date:     11/15/19
//-----------------------------------------------------------------------------

#include <fstream>

#include "TestScanner.h"
#include "TestParser.h"
#include "TestTree.h"

//Change this to either run all tests or just the scanner test. [0, 1]
#define PRIMARY_TEST_ONLY 1

bool parseArgs(int argc, char* argv[]);
void handleFileRedirOrKeyboardSim();
bool readFromFile(const std::string path);

std::string fileData;
std::string fileName;
std::string fileNameNoExtension;

//=============================================================================

int main(int argc, char* argv[]) {

    //Handle file and command line arguments
    bool success = false;

    fileNameNoExtension = "kb";

    //Handle either simulate keyboard input or file redirection
    if(argc == 1) {
       handleFileRedirOrKeyboardSim();
       success = true;
    }

    //Otherwise need to load a file
    else {
       //Get the file name.
       if(!parseArgs(argc, argv)) {
           std::cerr << "Error: Function: parseArgs(int, char**)" << std::endl;
           return 0;
       }

       //Read entire file contents into fileString
       if(!readFromFile(fileName)) {
           std::cerr << "Error: File could not be opened" << std::endl;
           return 0;
       }
       success = true;
    }

    //Remove extension from file name
    std::string::size_type pos = fileNameNoExtension.find_last_of(".");
    if(pos != std::string::npos) {            
        fileNameNoExtension.resize(fileNameNoExtension.size() - 5);
    }

    //-----

    //Testing:
	//readFromFile("testfile.fs19");

    Parser parser;
    parser.sendFileDataToScanner(fileData);

    ParseTree parseTree = parser.parse();

    parseTree.getOutFileName(fileNameNoExtension);
    
    //testPrintParseTree(parseTree);

	parseTree.staticSemanticsTraversal(parseTree.root);
	//std::cout << "Valid Program" << std::endl;

    parseTree.openOutFileStream();
    
	parseTree.codeGenTraversal(parseTree.root);

	parseTree.printUsedVars();

    parseTree.closeOutFileStream();

    return 0;
}

//=============================================================================

bool parseArgs(int argc, char* argv[]) {
    if(argc > 2){
        std::cerr << "Error: Too many arguments." << std::endl;
        return false;
    }

    /* 
    This is the case in which the user has passed the name of a file without
    the file extension appended to the end of it
    */
    else if(argc == 2) {
        //Retrieve the file name from the argument list
        fileName = std::string(argv[1]);
        fileNameNoExtension = std::string(argv[1]);
    
        //Find the last occurence of '.' 
        std::string::size_type pos = fileName.find_last_of(".");

        /*
        If the position is not the end of the string, append the .fs19 ext.
        This also handles the case where a filename contains other periods.
        */
        if(pos != std::string::npos) {            
            std::string extension = fileName.substr(pos + 1);
            if(extension != "fs19") {
                fileName.append(".fs19");
            }
        }
        else {
            fileName.append(".fs19");
        }

        //DEBUG
        //std::cout << cliInput << std::endl;
        
        return true;
    }
    else {
        handleFileRedirOrKeyboardSim();
        return true;
    }

    return false;
}

void handleFileRedirOrKeyboardSim() {
    /*
    Read entire cin buffer. Also handles simulated keyboard input.
    press "Enter" then "Ctrl + d" to end keyboard input.
    */
    char c = 0;
    while(std::cin.get(c)) {
        fileData.push_back(c);
    }

    //DEBUG
    //std::cout << cliInput;

    //Clear the std::cin buffer.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max());
    return;
}

bool readFromFile(const std::string path) {
    std::fstream inFile(path.c_str(), std::ios_base::in);

    if(!inFile.good()) {
        inFile.close();
        return false;
    }

    char c = 0;
    while(inFile.get(c)) {
        fileData.push_back(c);
    }

    inFile.close();

    return true;
}