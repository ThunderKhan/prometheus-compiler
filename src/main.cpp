#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tokenization.hpp"
#include "parser.hpp"

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream out;

    out << "global _start\n";
    out << "_start:\n";

    std::size_t i = 0;

    while (i < tokens.size()) {

        if (tokens[i].type == TokenType::kw_emit) {

            if (i + 2 >= tokens.size()) {
                std::cerr << "Incomplete emit statement\n";
                std::exit(EXIT_FAILURE);
            }

            if (tokens[i + 1].type != TokenType::int_lit) {
                std::cerr << "emit expects an integer literal\n";
                std::exit(EXIT_FAILURE);
            }

            if (tokens[i + 2].type != TokenType::semi) {
                std::cerr << "Missing ';' after emit\n";
                std::exit(EXIT_FAILURE);
            }

            // exit syscall
            out << "    mov rax, 60\n";
            out << "    mov rdi, " << tokens[i + 1].value.value() << "\n";
            out << "    syscall\n";

            i += 3;
            continue;
        }

        std::cerr << "Unexpected token in top-level\n";
        std::exit(EXIT_FAILURE);
    }

    return out.str();
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Incorrect usage.\n";
        std::cerr << "prometheus <input.pr>\n";
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::ifstream input(argv[1]);
        if (!input) {
            std::cerr << "Could not open file\n";
            return EXIT_FAILURE;
        }

        std::stringstream ss;
        ss << input.rdbuf();
        contents = ss.str();
    }

    Tokenizer tokenizer(contents);
    std::vector<Token> tokens = tokenizer.tokenize();

    {
        std::ofstream file("out.asm");
        if (!file) {
            std::cerr << "Could not write out.asm\n";
            return EXIT_FAILURE;
        }

        file << tokens_to_asm(tokens);
    }

    if (std::system("nasm -f elf64 out.asm") != 0) {
        std::cerr << "nasm failed\n";
        return EXIT_FAILURE;
    }

    if (std::system("ld -o out out.o") != 0) {
        std::cerr << "linking failed\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}