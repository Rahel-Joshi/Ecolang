#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>  // For std::any_of
#include <unordered_map>

#include "lexer.h"

// ==================== AST Node Definitions ====================

// Base class for all AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
};

// AST Node for Number literals
struct NumberNode : public ASTNode {
    std::string value;

    explicit NumberNode(std::string value) : value(value) {}
};

// AST Node for Variables
struct VariableNode : public ASTNode {
    std::string name;

    explicit VariableNode(std::string name) : name(name) {}
};

// AST Node for Binary Operations (e.g., addition, subtraction)
struct BinaryOpNode : public ASTNode {
    Token op;               // Operator token: +, -, *, /
    ASTNode* left;          // Left operand
    ASTNode* right;         // Right operand

    BinaryOpNode(Token op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}
};

// AST Node for Assignments
struct AssignmentNode : public ASTNode {
    std::string variable;
    ASTNode* expression;  // Right-hand side expression

    AssignmentNode(std::string variable, ASTNode* expression)
        : variable(variable), expression(expression) {}
};

// AST Node for If Statements
struct IfNode : public ASTNode {
    ASTNode* condition;
    ASTNode* thenBranch;
    ASTNode* elseBranch;

    IfNode(ASTNode* condition, ASTNode* thenBranch, ASTNode* elseBranch = nullptr)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
};

// AST Node for While Loops
struct WhileNode : public ASTNode {
    ASTNode* condition;
    ASTNode* body;

    WhileNode(ASTNode* condition, ASTNode* body)
        : condition(condition), body(body) {}
};

// AST Node for Print Statements
struct PrintNode : public ASTNode {
    ASTNode* expression;

    explicit PrintNode(ASTNode* expression) : expression(expression) {}
};

// AST Node for Blocks of Statements
struct BlockNode : public ASTNode {
    std::vector<ASTNode*> statements;

    explicit BlockNode(const std::vector<ASTNode*>& statements)
        : statements(statements) {}
};

// AST Node for Comparisons
struct CompareNode : public ASTNode {
    ASTNode* leftSide;
    Token compare;
    ASTNode* rightSide;

    CompareNode(Token compare, ASTNode* leftSide, ASTNode* rightSide)
        : compare(compare), leftSide(leftSide), rightSide(rightSide) {}
};

// ==================== Parser ====================

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens)
        : tokens(tokens), pos(0) {}

    // Parses the entire program (multiple statements)
    ASTNode* parseProgram() {
        std::vector<ASTNode*> statements;

        while (currentToken().type != END_OF_FILE) {
            ASTNode* statement = parseStatement();
            if (statement) {
                statements.push_back(statement);
            } else {
                // Handle parse error
                std::cerr << "Error parsing statement\n";
                break;
            }
        }

        return new BlockNode(statements);
    }

    // Parses a single statement
    ASTNode* parseStatement() {
        switch (currentToken().type) {
            case IDENTIFIER:
                return parseAssignment();
            case IF:
                return parseConditional();
            case WHILE:
                return parseWhile();
            case LBRACE:
                return parseBlock();
            case NUMBER:
            case LPAREN:
                return parseExpression();
            case PRINT:
                return parsePrint();
            default:
                std::cerr << "Error! Unexpected token in statement: " << currentToken().value << "\n";
                return nullptr;
        }
    }

    // Parses an expression
    ASTNode* parseExpression() {
        ASTNode* left = parseTerm();

        while (isCurrentToken({PLUS, MINUS})) {
            Token op = currentToken();
            advance();
            ASTNode* right = parseTerm();
            left = new BinaryOpNode(op, left, right);
        }

        if (!left) {
            std::cerr << "Error! Invalid expression\n";
        }

        return left;
    }

private:
    const std::vector<Token>& tokens;
    size_t pos;

    // Helper functions
    Token currentToken() const {
        return tokens[pos];
    }

    void advance() {
        if (pos < tokens.size() - 1) {
            pos++;
        }
    }

    bool isCurrentToken(const std::initializer_list<TokenType>& types) const {
        return std::any_of(types.begin(), types.end(),
                           [&](TokenType type) { return currentToken().type == type; });
    }

    void expectToken(TokenType expectedType, const std::string& errorMessage) {
        if (currentToken().type != expectedType) {
            std::cerr << "Error! " << errorMessage << ", found token: " << currentToken().value << "\n";
        }
        advance();
    }

    // Parse terms, factors, etc.
    ASTNode* parseTerm() {
        ASTNode* left = parseFactor();

        while (isCurrentToken({MULTIPLY, DIVIDE})) {
            Token op = currentToken();
            advance();
            ASTNode* right = parseFactor();
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
            expectToken(RPAREN, "Expected ')' after expression");
            return exp;
        }

        std::cerr << "Error! Unexpected token in factor: " << token.value << "\n";
        return nullptr;
    }

    // Parse assignment
    ASTNode* parseAssignment() {
        Token var = currentToken();
        advance();
        expectToken(ASSIGN, "Expected '=' after identifier");

        ASTNode* exp = parseExpression();
        return new AssignmentNode(var.value, exp);
    }

    // Parse conditions for if/while
    ASTNode* parseCondition() {
        ASTNode* leftSide = parseExpression();
        if (isCurrentToken({GEQ, LEQ})) {
            Token compare = currentToken();
            advance();
            ASTNode* rightSide = parseExpression();
            return new CompareNode(compare, leftSide, rightSide);
        }
        return leftSide;
    }

    // Parse conditional statements (if/else)
    ASTNode* parseConditional() {
        advance();  // Move past 'if'
        expectToken(LPAREN, "Expected '(' after 'if'");

        ASTNode* condition = parseCondition();
        expectToken(RPAREN, "Expected ')' after if condition");

        ASTNode* thenBranch = nullptr;
        if (currentToken().type == LBRACE) {
            thenBranch = parseBlock();
        } else {
            thenBranch = parseStatement();
        }

        ASTNode* elseBranch = nullptr;
        if (isCurrentToken({ELSE})) {
            advance();
            if (currentToken().type == LBRACE) {
                elseBranch = parseBlock();
            } else {
                elseBranch = parseStatement();
            }
        }

        return new IfNode(condition, thenBranch, elseBranch);
    }

    // Parse while loops
    ASTNode* parseWhile() {
        advance();  // Move past 'while'
        expectToken(LPAREN, "Expected '(' after 'while'");

        ASTNode* condition = parseCondition();
        expectToken(RPAREN, "Expected ')' after while condition");

        ASTNode* body = nullptr;
        if (currentToken().type == LBRACE) {
            body = parseBlock();
        } else {
            body = parseStatement();
        }

        return new WhileNode(condition, body);
    }

    // Parse block of statements
    ASTNode* parseBlock() {
        expectToken(LBRACE, "Expected '{' to start block");

        std::vector<ASTNode*> statements;

        while (currentToken().type != RBRACE && currentToken().type != END_OF_FILE) {
            ASTNode* statement = parseStatement();
            if (statement) {
                statements.push_back(statement);
            } else {
                // Handle parse error
                std::cerr << "Error parsing statement in block\n";
                break;
            }
        }

        expectToken(RBRACE, "Expected '}' at end of block");
        return new BlockNode(statements);
    }

    // Parse print statements
    ASTNode* parsePrint() {
        advance();  // Move past 'print'
        ASTNode* expr = parseExpression();

        if (!expr) {
            std::cerr << "Error! Invalid print statement\n";
            return nullptr;
        }

        return new PrintNode(expr);
    }
};

// ==================== Evaluator ====================

// Symbol table to store variable values
std::unordered_map<std::string, int> symbolTable;

// Function to evaluate expressions and return integer values
int evaluateExpression(ASTNode* node);

void evaluateAST(ASTNode* node) {
    if (dynamic_cast<BlockNode*>(node)) {
        // Handle block node
        BlockNode* blockNode = static_cast<BlockNode*>(node);
        for (ASTNode* stmt : blockNode->statements) {
            evaluateAST(stmt);
        }
    }
    else if (dynamic_cast<AssignmentNode*>(node)) {
        // Handle assignment node
        AssignmentNode* assignNode = static_cast<AssignmentNode*>(node);
        int value = evaluateExpression(assignNode->expression);
        symbolTable[assignNode->variable] = value;
    }
    else if (dynamic_cast<PrintNode*>(node)) {
        // Handle print node
        PrintNode* printNode = static_cast<PrintNode*>(node);
        int value = evaluateExpression(printNode->expression);
        std::cout << value << std::endl;
    }
    else if (dynamic_cast<IfNode*>(node)) {
        // Handle if node
        IfNode* ifNode = static_cast<IfNode*>(node);
        int conditionValue = evaluateExpression(ifNode->condition);
        if (conditionValue) {
            evaluateAST(ifNode->thenBranch);
        } else if (ifNode->elseBranch) {
            evaluateAST(ifNode->elseBranch);
        }
    }
    else if (dynamic_cast<WhileNode*>(node)) {
        // Handle while node
        WhileNode* whileNode = static_cast<WhileNode*>(node);
        while (evaluateExpression(whileNode->condition)) {
            evaluateAST(whileNode->body);
        }
    }
    else {
        std::cerr << "Error! Unsupported AST Node\n";
    }
}

int evaluateExpression(ASTNode* node) {
    if (dynamic_cast<NumberNode*>(node)) {
        NumberNode* numNode = static_cast<NumberNode*>(node);
        return std::stoi(numNode->value);
    }
    else if (dynamic_cast<VariableNode*>(node)) {
        VariableNode* varNode = static_cast<VariableNode*>(node);
        if (symbolTable.find(varNode->name) != symbolTable.end()) {
            return symbolTable[varNode->name];
        } else {
            std::cerr << "Error! Undefined variable: " << varNode->name << std::endl;
            return 0;
        }
    }
    else if (dynamic_cast<BinaryOpNode*>(node)) {
        BinaryOpNode* binOpNode = static_cast<BinaryOpNode*>(node);
        int leftValue = evaluateExpression(binOpNode->left);
        int rightValue = evaluateExpression(binOpNode->right);
        switch (binOpNode->op.type) {
            case PLUS:
                return leftValue + rightValue;
            case MINUS:
                return leftValue - rightValue;
            case MULTIPLY:
                return leftValue * rightValue;
            case DIVIDE:
                if (rightValue == 0) {
                    std::cerr << "Error! Division by zero\n";
                    return 0;
                }
                return leftValue / rightValue;
            default:
                std::cerr << "Error! Unsupported binary operator\n";
                return 0;
        }
    }
    else if (dynamic_cast<CompareNode*>(node)) {
        CompareNode* compNode = static_cast<CompareNode*>(node);
        int leftValue = evaluateExpression(compNode->leftSide);
        int rightValue = evaluateExpression(compNode->rightSide);
        switch (compNode->compare.type) {
            case GEQ:
                return leftValue >= rightValue;
            case LEQ:
                return leftValue <= rightValue;
            // Handle other comparison operators if needed
            default:
                std::cerr << "Error! Unsupported comparison operator\n";
                return 0;
        }
    }
    else {
        std::cerr << "Error! Unsupported expression type\n";
        return 0;
    }
}

// ==================== Main Function ====================

// Function to load source code from a file
std::string loadSourceCode(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the entire file into the buffer
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No source file provided\n";
        return 1;
    }

    // Load the source code from the provided file
    std::string sourceCode = loadSourceCode(argv[1]);

    if (sourceCode.empty()) {
        std::cerr << "Error: Empty or invalid source file\n";
        return 1;
    }

    // Tokenize the source code
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    // Parse the tokens into an AST
    Parser parser(tokens);
    ASTNode* root = parser.parseProgram();

    // Evaluate the AST
    if (root) {
        evaluateAST(root);  // Run the program by evaluating the root node
    } else {
        std::cerr << "Error: Parsing failed\n";
    }

    return 0;
}