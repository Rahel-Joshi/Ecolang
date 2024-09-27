#include <iostream>
#include <vector>
#include <string>

// Define token types
enum TokenType {
    IDENTIFIER, // Variable names (x, y, etc.)
    NUMBER,     // Numbers (e.g., 10, 3.14)
    PLUS,       // +
    MINUS,      // -
    MULTIPLY,   // *
    DIVIDE,     // /
    ASSIGN,     // =
    IF,         // 'if' conditional
    WHILE, 
    ELSE, 
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    GEQ,
    LEQ,
    END         // End of input
};

// Token structure
struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value = "") {
        this->type = type;
        this->value = value;
    }
};

// Lexer class (barebones)
class Lexer {
public:
    // Constructor that takes the input
    explicit Lexer(const std::string &input) {
        this->input = input;
        this->pos = 0;
    }

    // Function to tokenize the input (barebones, not implemented yet)
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        int l = 0;
        int size = input.size();

        for (int r = 0; r < size; r++) {
            if ((r == size - 1 || this->input.at(r + 1) == ' ') && this->input.at(r) != ' ') {
                // either r is pointing at eow or next char is ' ' and char r is pointing to is not a ' '
                std::string word = this->input.substr(l, r - l + 1);
                // case of number
                if (word == "+") {
                    tokens.push_back(Token(PLUS));
                }
                else if (word == "-") {
                    tokens.push_back(Token(MINUS));
                }
                else if (word == "*") {
                    tokens.push_back(Token(MULTIPLY));
                }
                else if (word == "/") {
                    tokens.push_back(Token(DIVIDE));
                }
                else if (word == "=") {
                    tokens.push_back(Token(ASSIGN));
                }
                // else if (word == "(") {
                //     tokens.push_back(Token(LPAREN));
                // }
                // else if (word == ")") {
                //     tokens.push_back(Token(RPAREN));
                // }
                else {
                    if (isNumber(word)) {
                        tokens.push_back(Token(NUMBER, word));
                    }
                    else {
                        tokens.push_back(Token(IDENTIFIER, word));
                    }
                }
            }
            else if (this->input.at(r) == ' ') {
                l = r+1;
                // l will always point to first non space
            }
        }
        // TODO: Implement tokenization logic here
        tokens.push_back(Token(END)); // Always end with END token
        return tokens;
    }

private:
    std::string input; // Input string
    size_t pos;        // Current position in the input

    bool isNumber(std::string &word) {
        for (char c : word) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

    // TODO: Implement helper functions like identifier() and number() here
};

// Helper function to display tokens (for debugging)
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

int main() {
    testLexer();  // Run lexer test
    return 0;
}
