#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <fstream>
#include <memory>
#include <variant>

#include "ast.hpp"

class Compiler {
    public:
    Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename);
    private:
    std::unique_ptr<Node::AST> ast_; 
    std::string filename_;
    std::ofstream fs_;
};

#endif // ASSEMBLY_HPP

