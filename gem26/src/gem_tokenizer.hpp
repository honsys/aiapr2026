#ifndef GEM_TOKENIZER_HPP
#define GEM_TOKENIZER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cctype>

enum GemTokenType {
    GEM_TOKEN_ID, GEM_TOKEN_NUMBER, GEM_TOKEN_STRING,
    GEM_TOKEN_LBRACKET, GEM_TOKEN_RBRACKET,
    GEM_TOKEN_LPAREN, GEM_TOKEN_RPAREN,
    GEM_TOKEN_COMMA, GEM_TOKEN_DOT, GEM_TOKEN_DOT_DOT, GEM_TOKEN_EQUALS,
    GEM_TOKEN_SEMICOLON,
    GEM_TOKEN_COLON,
    GEM_TOKEN_PLUS, GEM_TOKEN_MINUS, GEM_TOKEN_MUL, GEM_TOKEN_DIV,
    GEM_TOKEN_PLUS_EQUALS, GEM_TOKEN_MINUS_EQUALS, GEM_TOKEN_MUL_EQUALS, GEM_TOKEN_DIV_EQUALS,
    GEM_TOKEN_GREATER, GEM_TOKEN_LESS, GEM_TOKEN_COMPARE_EQUALS,
    GEM_TOKEN_KEYWORD_FUN, GEM_TOKEN_KEYWORD_OBJ, GEM_TOKEN_KEYWORD_END, GEM_TOKEN_KEYWORD_USE, GEM_TOKEN_KEYWORD_LIB, GEM_TOKEN_KEYWORD_ALIAS,
    GEM_TOKEN_KEYWORD_IF, GEM_TOKEN_KEYWORD_WHILE, GEM_TOKEN_KEYWORD_HIS,
    GEM_TOKEN_TYPE_INT, GEM_TOKEN_TYPE_DOUBLE, GEM_TOKEN_TYPE_STRING, GEM_TOKEN_TYPE_BOOL,
    GEM_TOKEN_EOF,
    GEM_TOKEN_UNKNOWN
};

struct GemToken {
    GemTokenType type;
    std::string text;
    double numberVal;
};

class GemTokenizer {
public:
    std::string src;
    size_t pos = 0;

    GemTokenizer(const std::string& s) : src(s) {}

    GemToken nextToken() {
        while (pos < src.size()) {
            if (isspace(src[pos])) {
                pos++;
            } else if (src[pos] == '#') {
                while (pos < src.size() && src[pos] != '\n') pos++;
            } else {
                break;
            }
        }
        if (pos >= src.size()) return {GEM_TOKEN_EOF, "", 0};

        char c = src[pos];

        if (isalpha(c) || c == '_' || c == '?') return identifier();
        if (isdigit(c) || (c == '-' && pos + 1 < src.size() && isdigit(src[pos+1]))) return number();
        if (c == '"') return string();

        pos++;
        switch (c) {
            case '[': return {GEM_TOKEN_LBRACKET, "[", 0};
            case ']': return {GEM_TOKEN_RBRACKET, "]", 0};
            case '(': return {GEM_TOKEN_LPAREN, "(", 0};
            case ')': return {GEM_TOKEN_RPAREN, ")", 0};
            case ',': return {GEM_TOKEN_COMMA, ",", 0};
            case '.': 
                if (pos < src.size() && src[pos] == '.') {
                    pos++;
                    return {GEM_TOKEN_DOT_DOT, "..", 0};
                }
                return {GEM_TOKEN_DOT, ".", 0};
            case ';': return {GEM_TOKEN_SEMICOLON, ";", 0};
            case '=': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {GEM_TOKEN_COMPARE_EQUALS, "==", 0};
                }
                return {GEM_TOKEN_EQUALS, "=", 0};
            case ':': return {GEM_TOKEN_COLON, ":", 0};
            case '+': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {GEM_TOKEN_PLUS_EQUALS, "+=", 0};
                }
                return {GEM_TOKEN_PLUS, "+", 0};
            case '-': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {GEM_TOKEN_MINUS_EQUALS, "-=", 0};
                }
                return {GEM_TOKEN_MINUS, "-", 0};
            case '*': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {GEM_TOKEN_MUL_EQUALS, "*=", 0};
                }
                return {GEM_TOKEN_MUL, "*", 0};
            case '/': 
                if (pos < src.size() && src[pos] == '=') {
                    pos++;
                    return {GEM_TOKEN_DIV_EQUALS, "/=", 0};
                }
                return {GEM_TOKEN_DIV, "/", 0};
            case '>': return {GEM_TOKEN_GREATER, ">", 0};
            case '<': return {GEM_TOKEN_LESS, "<", 0};
            default: return {GEM_TOKEN_UNKNOWN, std::string(1, c), 0};
        }
    }

    std::vector<GemToken> tokenize() {
        std::vector<GemToken> tokens;
        GemToken t = nextToken();
        while (t.type != GEM_TOKEN_EOF) {
            tokens.push_back(t);
            t = nextToken();
        }
        tokens.push_back(t); // Push EOF
        return tokens;
    }

private:
    GemToken identifier() {
        size_t start = pos;
        while (pos < src.size() && (isalnum(src[pos]) || src[pos] == '_' || src[pos] == '?')) pos++;
        std::string text = src.substr(start, pos - start);
        if (text == "fun") return {GEM_TOKEN_KEYWORD_FUN, text, 0};
        if (text == "obj") return {GEM_TOKEN_KEYWORD_OBJ, text, 0};
        if (text == "end") return {GEM_TOKEN_KEYWORD_END, text, 0};
        if (text == "use") return {GEM_TOKEN_KEYWORD_USE, text, 0};
        if (text == "lib") return {GEM_TOKEN_KEYWORD_LIB, text, 0};
        if (text == "alias") return {GEM_TOKEN_KEYWORD_ALIAS, text, 0};
        if (text == "if") return {GEM_TOKEN_KEYWORD_IF, text, 0};
        if (text == "while") return {GEM_TOKEN_KEYWORD_WHILE, text, 0};
        if (text == "his") return {GEM_TOKEN_KEYWORD_HIS, text, 0};
        if (text == "int") return {GEM_TOKEN_TYPE_INT, text, 0};
        if (text == "double") return {GEM_TOKEN_TYPE_DOUBLE, text, 0};
        if (text == "string") return {GEM_TOKEN_TYPE_STRING, text, 0};
        if (text == "bool") return {GEM_TOKEN_TYPE_BOOL, text, 0};
        return {GEM_TOKEN_ID, text, 0};
    }

    GemToken number() {
        size_t start = pos;
        if (src[pos] == '-') pos++;
        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) pos++;
        std::string text = src.substr(start, pos - start);
        double val = 0;
        try { val = std::stod(text); } catch(...) {}
        return {GEM_TOKEN_NUMBER, text, val};
    }

    GemToken string() {
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
        return {GEM_TOKEN_STRING, text, 0};
    }
};

#endif
