#ifndef MUNODE_H
#define MUNODE_H

#include <iostream>
#include "muParser.h"

// A class to parse & execute text files, muNode is best used as program interpreter.
class muNode
{
public:
    muNode(muNode* parent = nullptr) : parent(parent) {
        if (parent) {
            parent->addChild(this);
        }
    }

    muNode* parent;
    std::vector<muNode*> children;
    std::string expression;
    std::string condition;
    std::string name;
    std::string text;
    std::string filenode;
    bool condition_result=1;
    std::string text_result;
    int nr=0;
    bool execute_step_by_step=0;

    void addChild(muNode* child);
    double evaluateExpression(mu::Parser &parser);
    bool evaluateCondition(mu::Parser &parser);

    void addVar(std::string name, double &value, mu::Parser& parser);

    void setDebug(bool name,
                  bool condition, bool condition_result,
                  bool expression,
                  bool variables,
                  bool text);

    void printVars(int depth, mu::Parser& parser);
    std::string getNodeCommand(const mu::Parser &parser);
    void printNode(const mu::Parser &parser);

    bool hasParent() const; // Returns 1 if parent excists.
    bool hasChild() const ;
    bool hasLowerSibling() const ;
    bool hasUpperSibling() const ;
    muNode* getTopNode();
    muNode* getLowerSibling();
    muNode* getUpperSibling();
    bool isValid(muNode *node); // Declaration of isValid function

    void printhierarchy(int depth = 0);

    bool waitForEnterInput();

    muNode* getNextNode();
    muNode* getNextNodeByCondition(mu::Parser &parser);
    muNode* getPreviousNode();

private:

};

#endif // MUNODE_H
