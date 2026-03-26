#ifndef GEM_TOKENIZER_HPP
#define GEM_TOKENIZER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cctype>

enum TokenType {
    TOKEN_ID, TOKEN_NUMBER, TOKEN_STRING,
    TOKEN_LBRACKET, TOKEN_RBRACKET,
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_DOT_DOT, TOKEN_EQUALS,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_PLUS_EQUALS, TOKEN_MINUS_EQUALS, TOKEN_MUL_EQUALS, TOKEN_DIV_EQUALS,
    TOKEN_GREATER, TOKEN_LESS, TOKEN_COMPARE_EQUALS,
    TOKEN_KEYWORD_FUN, TOKEN_KEYWORD_OBJ, TOKEN_KEYWORD_END, TOKEN_KEYWORD_USE, TOKEN_KEYWORD_LIB, TOKEN_KEYWORD_ALIAS,
    TOKEN_KEYWORD_IF, TOKEN_KEYWORD_WHILE, TOKEN_KEYWORD_HIS,
    TOKEN_TYPE_INT, TOKEN_TYPE_DOUBLE, TOKEN_TYPE_STRING, TOKEN_TYPE_BOOL,
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    double numberVal;
};

class GemTokenizer {
public:
    std::string src;
    size_t pos = 0;

    GemTokenizer(const std::string& s) : src(s) {}

    Token nextToken() {
        while (pos < src.size()) {
            if (isspace(src[pos])) {
                pos++;
            } else if (src[pos] == '#') {
                while (pos < src.size() && src[pos] != '\n') pos++;
            } else {
                break;
            }
        }
        if (pos >= src.size()) return {TOKEN_EOF, "", 0};

        char c = src[pos];

        if (isalpha(c) || c == '_' || c == '?') return identifier();
        if (isdigit(c) || (c == '-' && pos + 1 < src.size() && isdigit(src[pos+1]))) return number();
        if (c == '"') return string();

        pos++;
        switch (c) {
            case '[': return {TOKEN_LBRACKET, "[", 0};
            case ']': return {TOKEN_RBRACKET, "]", 0};
            case '(': return {TOKEN_LPAREN, "(", 0};
            case ')': return {TOKEN_RPAREN, ")", 0};
            case ',': return {TOKEN_COMMA, ",", 0};
            case '.': 
                if (pos < src.size() && src[pos] == '.') {
                    pos++;
                    return {TOKEN_DOT_DOT, "..", 0};
                }
                return {TOKEN_DOT, ".", 0};
            case ';': return {TOKEN_SEMICOLON, ";", 0};
            case '=': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {TOKEN_COMPARE_EQUALS, "==", 0};
                }
                return {TOKEN_EQUALS, "=", 0};
            case ':': return {TOKEN_COLON, ":", 0};
            case '+': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {TOKEN_PLUS_EQUALS, "+=", 0};
                }
                return {TOKEN_PLUS, "+", 0};
            case '-': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {TOKEN_MINUS_EQUALS, "-=", 0};
                }
                return {TOKEN_MINUS, "-", 0};
            case '*': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {TOKEN_MUL_EQUALS, "*=", 0};
                }
                return {TOKEN_MUL, "*", 0};
            case '/': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {TOKEN_DIV_EQUALS, "/=", 0};
                }
                return {TOKEN_DIV, "/", 0};
            case '>': return {TOKEN_GREATER, ">", 0};
            case '<': return {TOKEN_LESS, "<", 0};
            default: return {TOKEN_UNKNOWN, std::string(1, c), 0};
        }
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token t = nextToken();
        while (t.type != TOKEN_EOF) {
            tokens.push_back(t);
            t = nextToken();
        }
        tokens.push_back(t); // Push EOF
        return tokens;
    }

private:
    Token identifier() {
        size_t start = pos;
        while (pos < src.size() && (isalnum(src[pos]) || src[pos] == '_' || src[pos] == '?')) pos++;
        std::string text = src.substr(start, pos - start);
        if (text == "fun") return {TOKEN_KEYWORD_FUN, text, 0};
        if (text == "obj") return {TOKEN_KEYWORD_OBJ, text, 0};
        if (text == "end") return {TOKEN_KEYWORD_END, text, 0};
        if (text == "use") return {TOKEN_KEYWORD_USE, text, 0};
        if (text == "lib") return {TOKEN_KEYWORD_LIB, text, 0};
        if (text == "alias") return {TOKEN_KEYWORD_ALIAS, text, 0};
        if (text == "if") return {TOKEN_KEYWORD_IF, text, 0};
        if (text == "while") return {TOKEN_KEYWORD_WHILE, text, 0};
        if (text == "his") return {TOKEN_KEYWORD_HIS, text, 0};
        if (text == "int") return {TOKEN_TYPE_INT, text, 0};
        if (text == "double") return {TOKEN_TYPE_DOUBLE, text, 0};
        if (text == "string") return {TOKEN_TYPE_STRING, text, 0};
        if (text == "bool") return {TOKEN_TYPE_BOOL, text, 0};
        return {TOKEN_ID, text, 0};
    }

    Token number() {
        size_t start = pos;
        if (src[pos] == '-') pos++;
        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) pos++;
        std::string text = src.substr(start, pos - start);
        double val = 0;
        try { val = std::stod(text); } catch(...) {}
        return {TOKEN_NUMBER, text, val};
    }

    Token string() {
        pos++; // skip "
        std::string text = "";
        while (pos < src.size() && src[pos] != '"') {
            if (src[pos] == '\\' && pos + 1 < src.size()) {
                pos++;
                if (src[pos] == '"') text += '"';
                else if (src[pos] == '\\') text += '\\';
                else if (src[pos] == 'n') text += '\n';
                else if (src[pos] == 'r') text += '\r';
                else if (src[pos] == 't') text += '\t';
                else text += src[pos];
            } else {
                text += src[pos];
            }
            pos++;
        }
        if (pos < src.size()) pos++; // skip closing "
        return {TOKEN_STRING, text, 0};
    }
};

#endif
