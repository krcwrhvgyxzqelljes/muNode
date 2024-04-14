#include "munode.h"
#include "mutext.h"

void muNode::addVar(std::string name, double &value, mu::Parser& parser){
    parser.DefineVar(name, &value);
}

void muNode::addChild(muNode* child) {
    children.push_back(child);
}

bool muNode::hasParent() const { // Returns 1 if parent excists.
    return parent != nullptr;
}

bool muNode::hasChild() const {
    if(this->children.size()>0){
        return 1;
    }
    return 0;
}

muNode* muNode::getTopNode() {
    muNode* currentNode = this;

    // Traverse up the parent pointers to find the top node
    while (currentNode->hasParent()) {
        currentNode = currentNode->parent;
    }

    return currentNode;
}

bool muNode::isValid(muNode *node) {
    if(node!=nullptr){
        return 1;
    }
    return 0;
}

muNode* muNode::getLowerSibling(){

    if (parent) {
        // Find the current node in the parent's children
        auto it = std::find(parent->children.begin(), parent->children.end(), this);

        // Check if the node is found and there are siblings after it
        if (it != parent->children.end() && std::distance(it, parent->children.end()) > 1) {
            // Return the pointer to the next node (lower sibling)
            return *(++it);
        }
    }
    return nullptr; // No lower sibling found or no parent
}

muNode* muNode::getUpperSibling(){

    if (parent) {
        // Find the current node in the parent's children
        auto it = std::find(parent->children.begin(), parent->children.end(), this);

        // Check if the node is found and there are siblings before it
        if (it != parent->children.end() && it != parent->children.begin()) {
            // Return the pointer to the previous node (upper sibling)
            return *(--it);
        }
    }
    return nullptr; // No upper sibling found or no parent
}

bool muNode::hasUpperSibling() const {

    if (parent) {
        std::cout<<"hasparent..."<<std::endl;
        // Find the current node in the parent's children
        auto it = std::find(parent->children.begin(), parent->children.end(), this);

        // Check if the node is found and there are siblings after it
        if (it != parent->children.end() && it != parent->children.begin()) {
            return true;
        }
    }
    return false;
}

bool muNode::hasLowerSibling() const {

    if (parent) {
        // Find the current node in the parent's children
        auto it = std::find(parent->children.begin(), parent->children.end(), this);

        // Check if the node is found and there are siblings after it
        if (it != parent->children.end() && std::distance(it, parent->children.end()) > 1) {
            return true;
        }
    }
    return false;
}

double muNode::evaluateExpression( mu::Parser& parser) {

    if(expression==""){
        return 0;
    }

    // Set expression in the parser
    parser.SetExpr(expression);

    try {
        // Evaluate the expression
        double result = parser.Eval();

        return result;

    } catch (const mu::Parser::exception_type& e) {
        // Handle evaluation errors
        std::cerr << "Evaluation error: " << e.GetMsg() << std::endl;
        return std::numeric_limits<double>::quiet_NaN(); // Return NaN for error
    }
}

bool muNode::evaluateCondition( mu::Parser& parser) {

    if(condition==""){
        return 0;
    }

    // Set condition in the parser
    parser.SetExpr(condition);

    try {
        // Evaluate the condition
        bool result = parser.Eval() != 0.0;

        return result;

    } catch (const mu::Parser::exception_type& e) {
        // Handle evaluation errors
        std::cerr << "Evaluation error: " << e.GetMsg() << std::endl;
        return false; // Return false for error
    }
}

bool muNode::waitForEnterInput() {
    std::cout << "Press Enter to continue..." << std::endl;

    // Clear previous inputs
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Wait for a key press
    char ch = std::cin.get();

    // Check if Enter key was pressed
    if (ch == '\n') {
        return true;  // Enter key was pressed
    } else {
        return false;  // Another key was pressed
    }
}

void muNode::printVars(int depth, mu::Parser& parser){
    // Print spaces based on depth
    for (int i = 0; i < depth; ++i) {
        std::cout << "\t";
    }
    for (const auto& var : parser.GetVar()) {
        std::cout << var.first << " = " << *(var.second) << ", ";
    }
    std::cout<<std::endl;
}

std::string muNode::getNodeCommand(const mu::Parser &parser){

    text_result=text;

    // Add items to content filter.
    std::vector<std::string> skiplist={"while", "$", "#"};
    for(uint i=0; i<skiplist.size(); i++){
        bool res=muText().contains_string(text_result,skiplist.at(i));
        if(res){
            text_result="";
        }
    }

    text_result=muText().solve_vartext(text_result,'[',']',parser,3);
    text_result=muText().removeCharacter(text_result,'}');

    if(text_result.size()>0){
        return text_result;
    }
    return "";
}

void muNode::printNode(const mu::Parser &parser){

    if(isValid(this)){
        std::cout<<" name:"<<name+"\t"<<" command:"<<getNodeCommand(parser)<<std::endl;
    }
}

muNode* muNode::getPreviousNode() {

    if(hasUpperSibling()){
        return getUpperSibling();
    }
    if(hasParent()){
        return parent;
    }
    return getTopNode();
}

muNode* muNode::getNextNodeByCondition(mu::Parser &parser) {

    // Check only the condition.
    condition_result=evaluateCondition(parser);

    if(condition.size()>0){ // Forward search in the node tree. If condition has a value.

        if(condition_result==0){ // Skip this node. Get lowersibling or else parent.
            if (hasLowerSibling()) {
                return getLowerSibling();
            }
            if (hasParent()){
                return parent;
            }
            return nullptr;
        }
    }

    // If condition was not met, update the expression values.
    evaluateExpression(parser);

    if (hasChild()) { // Next node is first child.
        return children.front();
    }

    if (hasLowerSibling()) { // Next node is first lower sibling.
        return getLowerSibling();
    }

    muNode* N = this; // No childs or lower sibling, find parent, then find lower sibling.
    while (N->hasParent()) {
        N = N->parent;

        if(N->condition.size()>0 && N->condition_result==1){
            return N;
        }
        if (N->hasLowerSibling()) {
            return N->getLowerSibling();
        }
    }

    return nullptr;
}

muNode* muNode::getNextNode() {
    if (hasChild()) {
        return children.front();
    }

    if (hasLowerSibling()) {
        return getLowerSibling();
    }

    muNode* currentNode = this;

    while (currentNode->hasParent()) {
        currentNode = currentNode->parent;
        if (currentNode->hasLowerSibling()) {
            return currentNode->getLowerSibling();
        }
    }

    return nullptr;
}

void muNode::printhierarchy(int depth) {
    if (!this->hasParent()) {
        std::cout << "" << std::endl;
        std::cout << "Node hierarchy:" << std::endl;
    }

    // Print current node
    for (int i = 0; i < depth - 1; ++i) {
        std::cout << "|   ";
    }

    std::cout << "`-- ";

    std::cout << "Name: ";
    if (!name.empty()) {
        std::cout << name;
    } else {
        std::cout << "";
    }

    std::cout << ", Condition: ";
    if (!condition.empty()) {
        std::cout << condition;
    } else {
        std::cout << "";
    }

    std::cout << ", Expression: ";
    if (!expression.empty()) {
        std::cout << expression;
    } else {
        std::cout << "";
    }

    std::cout << ", Text: ";
    if (!text.empty()) {
        std::cout << text;
    } else {
        std::cout << "";
    }

    std::cout << std::endl;

    // Recursively print children
    for (size_t i = 0; i < children.size(); ++i) {
        children[i]->printhierarchy(depth + 1);
    }

    // Add a newline at the end of the hierarchy
    if (!parent) {  // If this is the root node
        std::cout << std::endl;
    }
}








