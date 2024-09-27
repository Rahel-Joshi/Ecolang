#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "lexer.h"  // Include the lexer so we can access Token and TokenType

// AST Base Class
struct ASTNode {
    virtual ~ASTNode() = default;
};

// AST Node for Binary Operations (e.g., 10 + 5)
struct BinaryOpNode : public ASTNode {
    Token op;               // Operator token: +, -, *, /
    ASTNode* left;          // Left operand (can be another expression)
    ASTNode* right;         // Right operand (can be another expression)

    BinaryOpNode(Token op, ASTNode* left, ASTNode* right);
};

// AST Node for Number (Leaf Node)
struct NumberNode : public ASTNode {
    std::string value;

    explicit NumberNode(std::string value);
};

// AST Node for Variable (Leaf Node)
struct VariableNode : public ASTNode {
    std::string name;

    explicit VariableNode(std::string name);
};

// AST Node for Assignment (e.g., x = 10)
struct AssignmentNode : public ASTNode {
    std::string variable;
    ASTNode* expression;  // Right-hand side of the assignment

    AssignmentNode(std::string variable, ASTNode* expression);
};

// AST Node for "if" statements
struct IfNode : public ASTNode {
    ASTNode* condition;
    ASTNode* thenBranch;
    ASTNode* elseBranch;

    IfNode(ASTNode* condition, ASTNode* thenBranch, ASTNode* elseBranch = nullptr);
};

// AST Node for Comparison (e.g., x >= y)
struct CompareNode : public ASTNode {
    ASTNode* leftSide;
    Token compare;
    ASTNode* rightSide;

    CompareNode(Token compare, ASTNode* leftSide, ASTNode* rightSide);
};

// AST Node for "while" loops
struct WhileNode : public ASTNode {
    ASTNode* condition;
    ASTNode* body;

    WhileNode(ASTNode* condition, ASTNode* body);
};

// AST Node for Print statements
struct PrintNode : public ASTNode {
    ASTNode* expression;

    explicit PrintNode(ASTNode* expression);
};

// AST Node for Block of Statements
struct BlockNode : public ASTNode {
    std::vector<ASTNode*> statements;

    explicit BlockNode(const std::vector<ASTNode*>& statements);
};

struct ASTNode;
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    ASTNode* parseStatement();
    ASTNode* parseExpression();

private:
    const std::vector<Token>& tokens;
    size_t pos;
    
    Token currentToken() const;
    void advance();
    bool isCurrentToken(const std::initializer_list<TokenType>& types) const;

    ASTNode* parseTerm();
    ASTNode* parseFactor();
    ASTNode* parseAssignment();
    ASTNode* parseCondition();
    ASTNode* parseConditional();
    ASTNode* parseWhile();
    ASTNode* parseBlock();
    ASTNode* parsePrint();
    void expectToken(TokenType expectedType, const std::string& errorMessage);
};


#endif  // PARSER_H
