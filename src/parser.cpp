#include "parser.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::string& cmd) : command(cmd), pos(0) {}

void Parser::skip_whitespace() {
    while (pos < command.size() && std::isspace(command[pos])) ++pos;
}

std::string Parser::next_token() {
    skip_whitespace();
    size_t start = pos;
    while (pos < command.size() && !std::isspace(command[pos]) && command[pos] != ',' && command[pos] != ')' && command[pos] != '(') ++pos;
    return command.substr(start, pos - start);
}

ParseResult Parser::parse() {
    skip_whitespace();
    if (command.substr(pos) == "off") {
        auto seq = std::make_shared<SequenceNode>();
        auto off = std::make_shared<OffNode>();
        off->dwell = 0;
        seq->steps.push_back(off);
        return {seq, 1};
    }

    // Handle simple on/off as special
    if (command.substr(pos, 2) == "on ") {
        pos += 3;
        auto par = std::make_shared<ParallelNode>();
        while (pos < command.size()) {
            skip_whitespace();
            auto [chip, name] = parse_led();
            auto act = std::make_shared<LedActionNode>();
            act->chip = chip;
            act->name = name;
            act->dwell = -1;
            par->subs.push_back(act);
            skip_whitespace();
            if (pos < command.size() && command[pos] == ',') ++pos;
        }
        auto seq = std::make_shared<SequenceNode>();
        seq->steps.push_back(par);
        return {seq, 1};
    }

    if (command.substr(pos, 3) == "off ") {
        pos += 4;
        if (pos >= command.size()) {
            auto seq = std::make_shared<SequenceNode>();
            auto off = std::make_shared<OffNode>();
            off->dwell = 0;
            seq->steps.push_back(off);
            return {seq, 1};
        }
        auto par = std::make_shared<ParallelNode>();
        while (pos < command.size()) {
            skip_whitespace();
            auto [chip, name] = parse_led();
            auto act = std::make_shared<LedActionNode>();
            act->chip = chip;
            act->name = name;
            act->dwell = 0;
            par->subs.push_back(act);
            skip_whitespace();
            if (pos < command.size() && command[pos] == ',') ++pos;
        }
        auto seq = std::make_shared<SequenceNode>();
        seq->steps.push_back(par);
        return {seq, 1};
    }

    // Normal sequence
    auto root = parse_sequence();
    int loops = parse_loop();
    return {root, loops};
}

std::shared_ptr<SequenceNode> Parser::parse_sequence() {
    auto seq = std::make_shared<SequenceNode>();
    while (pos < command.size()) {
        skip_whitespace();
        if (command[pos] == ',' || command[pos] == ')') break;
        auto step = parse_step();
        seq->steps.push_back(step);
    }
    return seq;
}

std::shared_ptr<Node> Parser::parse_step() {
    skip_whitespace();
    if (command[pos] == '(') {
        ++pos;
        auto par = std::make_shared<ParallelNode>();
        while (true) {
            skip_whitespace();
            if (command[pos] == ')') {
                ++pos;
                break;
            }
            auto sub = parse_step();
            par->subs.push_back(sub);
            skip_whitespace();
            if (command[pos] == ',') ++pos;
            else if (command[pos] != ')') throw std::runtime_error("Parse error: expected , or )");
        }
        return par;
    }

    std::string tok = next_token();
    if (tok == "OFF") {
        double d = parse_dwell();
        auto off = std::make_shared<OffNode>();
        off->dwell = (d == -1 ? -1 : d);
        return off;
    }

    // LED
    size_t colon = tok.find(':');
    std::string chip = "gpiochip0";
    std::string name = tok;
    if (colon != std::string::npos) {
        chip = tok.substr(0, colon);
        name = tok.substr(colon + 1);
    }
    skip_whitespace();
    std::string next = next_token();
    if (next == "ON") {
        next = next_token();
    } // ignore ON
    double d;
    if (next == "inf") d = -1;
    else d = std::stod(next);
    auto act = std::make_shared<LedActionNode>();
    act->chip = chip;
    act->name = name;
    act->dwell = d;
    return act;
}

double Parser::parse_dwell() {
    std::string tok = next_token();
    if (tok == "inf") return -1;
    return std::stod(tok);
}

std::pair<std::string, std::string> Parser::parse_led() {
    std::string tok = next_token();
    size_t colon = tok.find(':');
    if (colon == std::string::npos) return {"gpiochip0", tok};
    return {tok.substr(0, colon), tok.substr(colon + 1)};
}

int Parser::parse_loop() {
    skip_whitespace();
    if (pos >= command.size() || command.substr(pos, 5) != ", loop") return 1;
    pos += 6;
    skip_whitespace();
    std::string tok = next_token();
    if (tok == "forever") return -1;
    return std::stoi(tok);
}