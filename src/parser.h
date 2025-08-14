#ifndef PARSER_H
#define PARSER_H

#include "nodes.h"
#include <string>
#include <memory>

struct ParseResult {
    std::shared_ptr<SequenceNode> root;
    int loop_count; // -1 forever
};

class Parser {
public:
    Parser(const std::string& cmd);
    ParseResult parse();
private:
    std::string command;
    size_t pos;
    void skip_whitespace();
    std::string next_token();
    std::shared_ptr<Node> parse_step();
    std::shared_ptr<SequenceNode> parse_sequence();
    double parse_dwell();
    std::pair<std::string, std::string> parse_led();
    int parse_loop();
};

#endif