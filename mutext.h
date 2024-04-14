#ifndef MUTEXT_H
#define MUTEXT_H

#include <string>
#include "munode.h"
#include "muParser.h"

// A Class to perform string operations wich will help the muNode.
class muText
{
public:
    muText();

    void solve_sample_vartext();

    std::string extract_text_between_chars(const std::string& str, char startChar, char endChar);

    std::string solve_vartext(std::string text, char id_begin, char id_end, mu::Parser parser, int decimal_places);

    muNode* read_file_into_nodes_text(std::string filename);

    std::string removeCharacter(const std::string& str, char ch);

    void print_varmap(const std::map<std::string, double*> &varmap);

    void print_parser_varmap(const mu::Parser &parser);

    void process_nodes(muNode *node, std::map<std::string, double *> &varmap);
    void process_nodes_nr(muNode *node);

    void set_varmap_to_parser(std::map<std::string, double *> varmap, mu::Parser &parser);

    bool contains_string(const std::string string, const std::string word_to_find);

    void create_sample_file();
    void create_sample_subroutine_file();

    int read_file_into_string(std::string filename, std::string &content);
    int save_string_to_file(const std::string& filename, const std::string& content);
    std::string replaceNewlineWithSpace(const std::string& input);
    std::string extract_text_after_char(const std::string& str, char startChar);
};

#endif // MUTEXT_H
