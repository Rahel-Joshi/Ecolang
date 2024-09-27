#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

enum TokenType {
    IDENTIFIER, NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE,
    ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE,
    IF, ELSE, WHILE, PRINT,
    GEQ, LEQ, // For comparison operators
    END_OF_FILE
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

    std::string parseIdentifier();
    std::string parseNumber();
};

#endif // LEXER_H
