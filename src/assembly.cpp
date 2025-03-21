#include "assembly.hpp"

Compiler::Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename) : filename_(filename) {
    if (!ast) {
        throw std::invalid_argument("null ast");
    }
    ast_ = std::make_unique<Node::AST>(std::move(*ast));
    fs_.open(filename_);
    if (!fs_) {
        throw std::invalid_argument("error: unable to open output file");
    }
}