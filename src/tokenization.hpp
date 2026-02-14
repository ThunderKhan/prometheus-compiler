#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstdlib>
#include <cctype>

enum class TokenType {
    kw_emit,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    inline explicit Tokenizer(const std::string& src)
        : m_src(std::move(src))
    {
    }

    inline std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buf;

        while (peak().has_value()) {

            char c = peak().value();

            if (std::isspace(c)) {
                consume();
                continue;
            }

            if (std::isalpha(c)) {
                buf.clear();

                buf.push_back(consume());

                while (peak().has_value() && std::isalnum(peak().value())) {
                    buf.push_back(consume());
                }

                if (buf == "emit") {
                    tokens.push_back(Token{ TokenType::kw_emit, std::nullopt });
                } else {
                    std::cerr << "Unknown identifier: " << buf << "\n";
                    std::exit(1);
                }

                continue;
            }

            if (std::isdigit(c)) {
                buf.clear();

                buf.push_back(consume());

                while (peak().has_value() && std::isdigit(peak().value())) {
                    buf.push_back(consume());
                }

                tokens.push_back(Token{ TokenType::int_lit, buf });
                continue;
            }

            if (c == ';') {
                consume();
                tokens.push_back(Token{ TokenType::semi, std::nullopt });
                continue;
            }

            std::cerr << "Unexpected character: " << c << "\n";
            std::exit(1);
        }
        m_index = 0;
        return tokens;
    }


private:
    [[nodiscard]] std::optional<char> peak(int ahead = 0) const {
        std::size_t pos = m_index + ahead;
        if (pos >= m_src.size()) {
            return {};
        }
        return m_src[pos];
    }

    char consume() {
        return m_src[m_index++];
    }

    int m_index = 0;
    const std::string m_src;
};