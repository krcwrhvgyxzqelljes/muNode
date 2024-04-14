#include "munode.h"
#include "mutext.h"

void muText_solve_sample_vartext(){
    muText muT;
    muT.solve_sample_vartext();
}

void parser_test(){
    double a = 1;
    double b = 0;
    double c = 0;

    mu::Parser parser;
    parser.DefineVar("a", &a);
    parser.DefineVar("b", &b);
    parser.DefineVar("c", &c);

    muNode *node=new muNode();
    node->condition="";
    node->expression="10+a";

    std::cout<<"result condition:"<<node->evaluateCondition(parser)<<std::endl;
    std::cout<<"result expression:"<<node->evaluateExpression(parser)<<std::endl;
}

void muText_file_Test(){
    mu::Parser parser;
    std::map<std::string, double*> varmap;

    muText().create_sample_file();
    muText().create_sample_subroutine_file();

    muNode *node=muText().read_file_into_nodes_text("file.txt");
    muText().process_nodes(node,varmap);
    muText().process_nodes_nr(node);
    muText().print_varmap(varmap);

    muText().set_varmap_to_parser(varmap,parser);
    muText().print_parser_varmap(parser);

    node->printhierarchy(0);

    muNode* N = node;
    while(N->isValid(N)){
        N= N->getNextNodeByCondition(parser);
        N->printNode(parser);
    }
}

int main() {
    muText_file_Test();
    return 0;
}







