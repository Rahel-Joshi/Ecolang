#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>

// Define AST Node types
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct BinaryOpNode : public ASTNode {
    Token op;
    ASTNode* left;
    ASTNode* right;
    BinaryOpNode(Token op, ASTNode* left, ASTNode* right) : op(op), left(left), right(right) {}
};

struct NumberNode : public ASTNode {
    std::string value;
    explicit NumberNode(std::string value) : value(value) {}
};

// Parser class definition
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    ASTNode* parseExpression();

private:
    const std::vector<Token>& tokens;
    size_t pos;

    Token currentToken() const;
    void advance();
    ASTNode* parseTerm();
    ASTNode* parseFactor();
};

#endif // PARSER_H
