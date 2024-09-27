#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

enum TokenType {
    IDENTIFIER, NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN, IF, WHILE, ELSE, LBRACE, RBRACE, LPAREN, RPAREN, GEQ, LEQ, END
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value = "") : type(type), value(value) {}
};

class Lexer {
public:
    explicit Lexer(const std::string &input);
    std::vector<Token> tokenize();

private:
    std::string input;
    size_t pos;

    bool isNumber(std::string &word);
};

#endif // LEXER_H
