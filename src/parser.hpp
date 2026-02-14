#pragma once

#include <iostream>
#include <optional>
#include <vector>

#include "tokenization.hpp"

// ---- AST nodes ----

struct NodeExpr {
    Token int_lit;
};

struct NodeEmit {
    NodeExpr expr;
};

// ---- Parser ----

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens))
    {
    }

    // expr := int_lit
    std::optional<NodeExpr> parse_expr() {
        if (peak().has_value() &&
            peak()->type == TokenType::int_lit)
        {
            return NodeExpr{ consume() };
        }

        return {};
    }

    // statement := "emit" expr ";"
    std::optional<NodeEmit> parse_emit() {

        // consume 'emit'
        if (!peak().has_value() ||
            peak()->type != TokenType::kw_emit)
        {
            return {};
        }

        consume(); // eat 'emit'

        auto expr = parse_expr();
        if (!expr.has_value()) {
            std::cerr << "Expected expression after 'emit'\n";
            std::exit(EXIT_FAILURE);
        }

        if (!peak().has_value() ||
            peak()->type != TokenType::semi)
        {
            std::cerr << "Expected ';' after emit expression\n";
            std::exit(EXIT_FAILURE);
        }

        consume(); // eat ';'

        return NodeEmit{ expr.value() };
    }

    // entry point
    std::optional<NodeEmit> parse() {

        auto node = parse_emit();

        if (!node.has_value()) {
            std::cerr << "Expected 'emit' statement\n";
            std::exit(EXIT_FAILURE);
        }

        if (peak().has_value()) {
            std::cerr << "Unexpected tokens after statement\n";
            std::exit(EXIT_FAILURE);
        }

        return node;
    }

private:
    [[nodiscard]] std::optional<Token> peak(std::size_t ahead = 0) const {
        std::size_t pos = m_index + ahead;
        if (pos >= m_tokens.size())
            return {};
        return m_tokens[pos];
    }

    Token consume() {
        return m_tokens[m_index++];
    }

    std::size_t m_index = 0;
    std::vector<Token> m_tokens;
};