#include "mutext.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

muText::muText()
{

}

void muText::solve_sample_vartext(){
    double a=12.2, b=100.123, c=587.369;
    mu::Parser parser;
    parser.DefineVar("a",&a);
    parser.DefineVar("b",&b);
    parser.DefineVar("c",&c);

    std::string text="G0 X[a] Y[b] C[c]";

    std::cout<<"result text:"<<solve_vartext(text,'[',']',parser,3)<<std::endl;
}

std::string muText::solve_vartext(std::string text, char id_begin, char id_end, mu::Parser parser, int decimal_places){
    std::string result = text;
    std::string::size_type begin_pos = result.find(id_begin);

    while (begin_pos != std::string::npos) {
        std::string::size_type end_pos = result.find(id_end, begin_pos + 1);

        if (end_pos == std::string::npos) {
            // Handle error: No matching id_end found
            return "Error: No matching id_end found";
        }

        std::string expression = result.substr(begin_pos + 1, end_pos - begin_pos - 1);

        // Set the expression to the parser
        parser.SetExpr(expression);

        // Evaluate the expression
        double eval_result = parser.Eval();

        // Convert double to string with specified decimal places
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimal_places) << eval_result;
        std::string formatted_result = oss.str();

        // Replace the expression in the result string with the formatted result
        result.replace(begin_pos, end_pos - begin_pos + 1, formatted_result);

        // Find the next id_begin in the result string
        begin_pos = result.find(id_begin, begin_pos + formatted_result.length());
    }

    return result;
}

std::string removeMultipleSpaces(const std::string& input) {
    std::string result = input;

    // Remove extra spaces
    auto new_end = std::unique(result.begin(), result.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });

    result.erase(new_end, result.end());

    // Remove leading and trailing spaces
    size_t start = result.find_first_not_of(" ");
    size_t end = result.find_last_not_of(" ");

    if (start == std::string::npos || end == std::string::npos) {
        return "";  // No non-space characters
    }

    return result.substr(start, end - start + 1);
}

std::string muText::extract_text_between_chars(const std::string& str, char startChar, char endChar) {
    std::size_t startPos = str.find(startChar);
    if (startPos == std::string::npos) {
        return "";  // Start character not found
    }

    std::size_t endPos = str.find(endChar, startPos + 1);
    if (endPos == std::string::npos) {
        return "";  // End character not found
    }

    std::string res=str.substr(startPos + 1, endPos - startPos - 1);
    res=removeMultipleSpaces(res);
    return res;
}

int muText::read_file_into_string(std::string filename, std::string &content){
    // Open the file
    std::ifstream file(filename);

    // Check if the file is opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    // Read the file into a std::string
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Close the file
    file.close();

    // Print the content
    // std::cout << "File content:" << std::endl;
    // std::cout << content << std::endl;

    return 0;
}

int muText::save_string_to_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1; // Return error code
    }

    // Write content to the file
    file << content;

    file.close();

    return 0; // Return success code
}

std::string muText::replaceNewlineWithSpace(const std::string& input) {
    std::string result = input;
    std::replace(result.begin(), result.end(), '\n', ' ');
    return result;
}

std::string muText::extract_text_after_char(const std::string& str, char startChar) {
    std::size_t startPos = str.find(startChar);
    if (startPos == std::string::npos || startPos == str.length() - 1) {
        return "";  // Start character not found or found at the end
    }
    std::string res=str.substr(startPos + 1);
    res=removeMultipleSpaces(res);
    return res;
}

muNode* muText::read_file_into_nodes_text(std::string filename) {
    // Create the root node
    muNode* currentNode = new muNode();
    currentNode->text="Topology file node.";
    // muNode* currentNode = rootNode; // Current node to which new nodes will be added
    muNode* newNode;

    // Read the file into a string
    std::string fileContent;
    read_file_into_string(filename, fileContent);

    // Replace newline characters with spaces
    fileContent = replaceNewlineWithSpace(fileContent);

    // Remove multiple spaces
    fileContent = removeMultipleSpaces(fileContent);

    // Append a space at the end to simplify string processing
    fileContent += " ";

    std::size_t startPos = 0;
    std::size_t endPos = 0;

    bool isGcode=0;
    bool isMcode=0;
    bool isComment=0;
    bool isWhile=0;
    bool isVar=0;
    bool isFile=0;

    // Process the string
    while (startPos < fileContent.size()) {
        // Find the next space
        endPos = fileContent.find(' ', startPos);

        // Extract the substring between startPos and endPos
        std::string substring = fileContent.substr(startPos, endPos - startPos);

        // Debug print
        // std::cout << "substring: " << substring << std::endl;

        // Handle variables
        if (substring.front() == '<') {
            // std::cout << "adding node comment" << std::endl;
            newNode = new muNode(currentNode);
            isComment=false;
            isGcode=false;
            isMcode=false;
            isWhile=false;
            isVar=false;
            isFile=true;
        }

        // Handle variables
        if (substring.front() == '$') {
            // std::cout << "adding node comment" << std::endl;
            newNode = new muNode(currentNode);
            isComment=false;
            isGcode=false;
            isMcode=false;
            isWhile=false;
            isVar=true;
            isFile=false;
        }

        // Handle comments
        if (substring.front() == '#') {
            // std::cout << "adding node comment" << std::endl;
            newNode = new muNode(currentNode);
            isComment=true;
            isGcode=false;
            isMcode=false;
            isWhile=false;
            isVar=false;
            isFile=false;
        }

        // Handle vars
        if (substring.front() == '$') {
            // std::cout << "adding node var" << std::endl;
            // newNode = new muNode(currentNode);
            isComment=false;
            isGcode=false;
            isMcode=false;
            isWhile=false;
            isVar=true;
            isFile=false;
        }

        // Handle G-codes
        if (substring.front() == 'G') {
            // std::cout << "adding node G" << std::endl;
            newNode = new muNode(currentNode);
            isComment=false;
            isGcode=true;
            isMcode=false;
            isWhile=false;
            isVar=false;
            isFile=false;
        }

        // Handle M-codes
        if (substring.front() == 'M') {
            // std::cout << "adding node M" << std::endl;
            newNode = new muNode(currentNode);
            isComment=false;
            isGcode=false;
            isMcode=true;
            isWhile=false;
            isVar=false;
            isFile=false;
        }

        // Handle while.
        if (substring.front() == 'w') {
            // std::cout << "adding node while" << std::endl;
            newNode = new muNode(currentNode);
            currentNode=newNode;
            isComment=false;
            isGcode=false;
            isMcode=false;
            isWhile=true;
            isVar=false;
            isFile=false;
        }
        // Handle expression
        if (substring.back() == '}') {
            currentNode=currentNode->parent;
        }

        if(isComment){
            newNode->text.append(substring+" ");
        }
        if(isGcode){
            newNode->text.append(substring+" ");
        }
        if(isMcode){
            newNode->text.append(substring+" ");
        }
        if(isWhile){
            newNode->text.append(substring+" ");
        }
        if(isVar){
            newNode->text.append(substring+" ");
        }
        if(isFile){
            newNode->text.append(substring+" ");

            std::string filename=newNode->text;

            filename=removeCharacter(filename,'<');
            filename=removeCharacter(filename,'>');
            filename=removeCharacter(filename,'}');
            filename=removeMultipleSpaces(filename);

            // std::cout<<"file found, name :"<<filename<<std::endl;

            newNode->filenode=filename;
            muNode *node=read_file_into_nodes_text(filename);
            node->parent=newNode;
            newNode->addChild(node);

            isFile=0;
        }

        // Update startPos to the position after the space
        startPos = endPos + 1;
    }

    return currentNode;
}

void muText::print_varmap(const std::map<std::string, double*> &varmap){
    // Print the content of varm
    std::cout<<""<<std::endl;
    std::cout<<"Varmap:"<<std::endl;
    for (const auto& pair : varmap) {
        std::cout << "Key: " << pair.first << ", Value: " << *(pair.second) << std::endl;
    }
}

void muText::print_parser_varmap(const mu::Parser &parser){
    // Print the content of varm
    std::cout<<""<<std::endl;
    std::cout<<"Parser varmap:"<<std::endl;
    for (const auto& pair : parser.GetVar()) {
        std::cout << "Key: " << pair.first << ", Value: " << *(pair.second) << std::endl;
    }
}

std::string muText::removeCharacter(const std::string& str, char ch) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), ch), result.end());
    return result;
}

int nr=0;
void muText::process_nodes_nr(muNode *node){
    // std::cout << "Text: " << node->text << std::endl;
    node->nr = nr;
    node->name = "Node " + std::to_string(node->nr);

    // Recursively process children
    for (const auto& child : node->children) {
        nr++;
        process_nodes_nr(child);
    }
}

void muText::process_nodes(muNode *node, std::map<std::string, double*> &varmap){

    // Filter out condition.
    std::string condition=extract_text_between_chars(node->text,'(',')');
    node->condition=condition;
    if(condition.size()==0){
        // node->condition="0";
    }
    // std::cout<<"Found condition:"<< condition <<std::endl;

    // Filter out expression.
    std::string expression=extract_text_after_char(node->text,'{');
    expression=removeCharacter(expression,'}'); // If there is left.
    node->expression=expression;
    if(expression.size()==0){
        // node->expression="0";
    }
    // std::cout<<"Found expression:"<< expression <<std::endl;

    // Filter out vars.
    std::string key=extract_text_between_chars(node->text,'$','=');
    std::string valuetext=extract_text_after_char(node->text,'=');
    // std::cout<<"Found key:"<< key <<std::endl;
    // std::cout<<"Found value:"<< valuetext <<std::endl;
    double value=0;
    if(key.size()>0){
        value=std::stod(valuetext);
        double* ptr_value = new double(value);
        varmap.insert({key,ptr_value});
    }

    // Recursively process children
    for (const auto& child : node->children) {
        process_nodes(child, varmap);
    }
}

void muText::set_varmap_to_parser(std::map<std::string, double *> varmap, mu::Parser &parser){
    for (const auto& pair : varmap) {
        parser.DefineVar(pair.first,pair.second);
        // std::cout << "Key: " << pair.first << ", Value: " << *(pair.second) << std::endl;
    }
}

bool muText::contains_string(const std::string string, const std::string word_to_find){

    std::size_t pos=string.find(word_to_find);
    if(pos!=std::string::npos){
        return 1;
    }
    return 0;
}

void muText::create_sample_file(){
    std::string str = "# this is a comment line\n";
    str += "$x=0\n";
    str += "$y=0\n";
    str += "$z=0\n";
    str += "G0 X0 Y0 Z0\n";
    str += "<subroutine.txt>\n";
    str += "while(x<2){\n";
    str += "    x=x+1\n";
    str += "    G0 X[x] Y[y] Z1\n";
    str += "    while(y<2){\n";
    str += "        y=y+1\n";
    str += "        G2 X[x] Y[y] Z3\n";
    str += "        <subroutine.txt>\n";
    str += "        while( z<2 ){\n";
    str += "            z=z+1\n";
    str += "            G3 X[x] Y[y] Z[z]\n";
    str += "        }\n";
    str += "    }\n";
    str += "}\n";
    str += "G0 X0 Y0 Z0\n";
    str += "M30\n";

    // Save to file
    std::ofstream outFile("file.txt");
    if (outFile.is_open()) {
        outFile << str;
        outFile.close();
        std::cout << "File saved successfully." << std::endl;
    } else {
        std::cout << "Unable to open file." << std::endl;
    }
}

void muText::create_sample_subroutine_file(){
    std::string str = "M3\n";
    str += "G0 X1.5 Y1.5 Z8.0\n";
    str += "G0 X[x] Y[y] Z10\n";
    str += "G1 X[x+5] Y[y+5] Z25\n";
    str += "M5\n";

    // Save to file
    std::ofstream outFile("subroutine.txt");
    if (outFile.is_open()) {
        outFile << str;
        outFile.close();
        std::cout << "File saved successfully." << std::endl;
    } else {
        std::cout << "Unable to open file." << std::endl;
    }
}








