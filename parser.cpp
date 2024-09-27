#include <iostream>
#include <vector>
#include <string>

#include "lexer.h"

// AST Base Class
struct ASTNode {
    virtual ~ASTNode() = default;
};

// AST Node for Binary Operations (e.g., 10 + 5)
struct BinaryOpNode : public ASTNode {
    Token op;               // Operator token: +, -, *, /
    ASTNode* left;          // Left operand (can be another expression)
    ASTNode* right;         // Right operand (can be another expression)

    BinaryOpNode(Token op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}
};

// AST Node for Number (Leaf Node)
struct NumberNode : public ASTNode {
    std::string value;

    explicit NumberNode(std::string value) {
        this->value = value;
    }
};

// AST Node for Variable (Leaf Node)
struct VariableNode : public ASTNode {
    std::string name;

    explicit VariableNode(std::string name) {
        this->name = name;
    }
};

// AST Node for Assignment (e.g., x = 10)
struct AssignmentNode : public ASTNode {
    std::string variable;
    ASTNode* expression;  // Right-hand side of the assignment

    AssignmentNode(std::string variable, ASTNode* expression) {
        this->variable = variable;
        this->expression = expression;
    }
};

// AST Node for "if" statements
struct IfNode : public ASTNode {
    ASTNode* condition;
    ASTNode* thenBranch;
    ASTNode* elseBranch;

    IfNode(ASTNode* condition, ASTNode* thenBranch, ASTNode* elseBranch = nullptr) {
        this->condition = condition;
        this->thenBranch = thenBranch;
        this->elseBranch = elseBranch;
    }
};

struct CompareNode : public ASTNode {
    ASTNode *leftSide;
    Token compare;
    ASTNode *rightSide;

    CompareNode(Token compare, ASTNode *leftSide, ASTNode *rightSide)
        : compare(compare), leftSide(leftSide), rightSide(rightSide);
};

// AST Node for "while" loops
struct WhileNode : public ASTNode {
    ASTNode* condition;
    ASTNode* body;

    WhileNode(ASTNode* condition, ASTNode* body) {
        this->condition = condition;
        this->body = body;
    }
};

// Parser class
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) {
        this->tokens = &tokens;
        this->pos = 0;
    }

    // Parsep an expression
    ASTNode* parseExpression() {
        ASTNode *left = parseTerm();

        while (currentToken().type == PLUS || currentToken().type == MINUS) {
            Token op = currentToken();
            advance();
            ASTNode *right = parseTerm();

            left = new BinaryOpNode(op, left, right);
        }
        return left;
    }

private:
    const std::vector<Token>* tokens;
    size_t pos;

    Token currentToken() const {
        return (*tokens)[pos];
    }
    void advance() {
        pos++;
    }
    
    // Parse terms, factors, etc.
    ASTNode* parseTerm() {
        ASTNode *left = parseFactor();
        
        while (currentToken().type == MULTIPLY || currentToken().type == DIVIDE) {
            Token op = currentToken();
            advance();

            ASTNode *right = parseFactor();

            left = new BinaryOpNode(op, left, right);
        }
        return left;
    }
    ASTNode* parseFactor() {
        Token token = currentToken();
        if (token.type == NUMBER) {
            advance();
            return new NumberNode(token.value);
        } else if (token.type == IDENTIFIER) {
            advance();
            return new VariableNode(token.value);
        } else if (token.type == LPAREN) {
            advance();
            ASTNode* exp = parseExpression();
            if (currentToken().type != RPAREN) {
                std::cerr << "Error! Expected ') after expression\n";
                return nullptr;
            }
            advance();
            return exp;
        } else {
            std::cerr << "Error! Unexpected token\n";
            return nullptr;
        }
    }
    ASTNode *parseAssingment() {
        Token var = currentToken();
        if (var.type == IDENTIFIER) {
            advance();
            if (currentToken().type == ASSIGN) {
                advance();
                ASTNode *exp = parseExpression(); 
                return new AssignmentNode(var.value, exp);
            }
        }
        std::cerr << "Error! Invalid assignment\n";
        return nullptr;
    }

    ASTNode *parseCondition() {
        ASTNode *leftSide = parseExpression();

        if (currentToken().type == GEQ || currentToken().type == LEQ) {
            Token compare = currentToken();
            advance();

            ASTNode *rightSide = parseExpression();
            return new CompareNode(compare, leftSide, rightSide);
        }
        return leftSide;
    }

    ASTNode *parseConditional() {
        Token var = currentToken();
        if (var.type == IF) {
            advance();
            if (currentToken().type == LPAREN) {
                advance();
                ASTNode *condition = parseCondition();
                if (currentToken().type == RPAREN) {

                }
            }
        }
        std::cerr << "Error! Invalid conditional\n";
        return nullptr;
    }
};



