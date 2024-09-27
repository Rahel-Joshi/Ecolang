#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>  // For std::any_of

// ==================== Lexer and Token Definitions ====================

// Enum for different types of tokens
enum TokenType {
    IDENTIFIER, NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE,
    ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE,
    IF, ELSE, WHILE, PRINT,
    GEQ, LEQ, // For comparison operators
    END_OF_FILE
};

// Structure representing a token
struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value = "")
        : type(type), value(value) {}
};

// Lexer class to tokenize the input source code
class Lexer {
public:
    Lexer(const std::string& source)
        : source(source), pos(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < source.size()) {
            char currentChar = source[pos];

            if (std::isspace(currentChar)) {
                pos++;
                continue;
            }

            if (std::isalpha(currentChar)) {
                std::string identifier = parseIdentifier();
                if (identifier == "if") {
                    tokens.push_back(Token(IF));
                } else if (identifier == "else") {
                    tokens.push_back(Token(ELSE));
                } else if (identifier == "while") {
                    tokens.push_back(Token(WHILE));
                } else if (identifier == "print") {
                    tokens.push_back(Token(PRINT));
                } else {
                    tokens.push_back(Token(IDENTIFIER, identifier));
                }
                continue;
            }

            if (std::isdigit(currentChar)) {
                std::string number = parseNumber();
                tokens.push_back(Token(NUMBER, number));
                continue;
            }

            switch (currentChar) {
                case '+':
                    tokens.push_back(Token(PLUS));
                    pos++;
                    break;
                case '-':
                    tokens.push_back(Token(MINUS));
                    pos++;
                    break;
                case '*':
                    tokens.push_back(Token(MULTIPLY));
                    pos++;
                    break;
                case '/':
                    tokens.push_back(Token(DIVIDE));
                    pos++;
                    break;
                case '=':
                    // Check for '==' (equality operator)
                    if (pos + 1 < source.size() && source[pos + 1] == '=') {
                        // For simplicity, we'll treat '==' as ASSIGN
                        tokens.push_back(Token(ASSIGN));
                        pos += 2;
                    } else {
                        tokens.push_back(Token(ASSIGN));
                        pos++;
                    }
                    break;
                case '>':
                    if (pos + 1 < source.size() && source[pos + 1] == '=') {
                        tokens.push_back(Token(GEQ));
                        pos += 2;
                    } else {
                        // For simplicity, we'll treat '>' as GEQ
                        tokens.push_back(Token(GEQ));
                        pos++;
                    }
                    break;
                case '<':
                    if (pos + 1 < source.size() && source[pos + 1] == '=') {
                        tokens.push_back(Token(LEQ));
                        pos += 2;
                    } else {
                        // For simplicity, we'll treat '<' as LEQ
                        tokens.push_back(Token(LEQ));
                        pos++;
                    }
                    break;
                case '(':
                    tokens.push_back(Token(LPAREN));
                    pos++;
                    break;
                case ')':
                    tokens.push_back(Token(RPAREN));
                    pos++;
                    break;
                case '{':
                    tokens.push_back(Token(LBRACE));
                    pos++;
                    break;
                case '}':
                    tokens.push_back(Token(RBRACE));
                    pos++;
                    break;
                default:
                    std::cerr << "Unknown character: " << currentChar << std::endl;
                    pos++;
                    break;
            }
        }

        tokens.push_back(Token(END_OF_FILE));
        return tokens;
    }

private:
    std::string source;
    size_t pos;

    std::string parseIdentifier() {
        std::string result;
        while (pos < source.size() && (std::isalnum(source[pos]) || source[pos] == '_')) {
            result += source[pos++];
        }
        return result;
    }

    std::string parseNumber() {
        std::string result;
        while (pos < source.size() && std::isdigit(source[pos])) {
            result += source[pos++];
        }
        return result;
    }
};

void displayTokens(const std::vector<Token>& tokens) {
    for (Token token : tokens) {
        std::cout << "Token(Type: " << token.type;
        std::cout << ", Value: " << token.value << ")\n"; 
    }
}

// Unit test (barebones)
void testLexer() {
    std::string input = "x = 10 + y * (20 - 5)";

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    displayTokens(tokens);  // Output tokens for debugging (nothing here yet)
}



