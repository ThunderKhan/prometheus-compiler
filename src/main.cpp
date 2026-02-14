#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tokenization.hpp"

std::vector<Token> tokenize(const std::string& str) {
    
}

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

            out << "    mov rax, 60\n";
            out << "    mov rdi, " << tokens[i + 1].value.value() << "\n";
            out << "    syscall\n";

            i += 3;   // consume: emit, int, ;
            continue;
        }

        std::cerr << "Unexpected token in top-level\n";
        std::exit(EXIT_FAILURE);
    }

    return out.str();
}

int main(int argc,  char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "prometheus <intput.pr>" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string contents; 
    {
        std::fstream input(argv[1], std::ios::in);
        if (!input) {
            std::cerr << "Could not open file\n";
            return EXIT_FAILURE;
        }

        std::stringstream contents_stream;
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }
    
    std::vector<Token> tokens = tokenize(contents);
    
    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    if (system("nasm -f elf64 out.asm") != 0) {
        std::cerr << "nasm failed\n";
        return EXIT_FAILURE;
    }

    if (system("ld -o out out.o") != 0) {
        std::cerr << "linking failed.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}