#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <iostream>
#include <memory>

#include "ast.hpp"
#include "token.hpp"

// clang-format off
class Analyzer
{
    public:
    Analyzer(const std::shared_ptr<Node::AST> &ast);
    void exprTypeSetter(const std::unique_ptr<Node::GenericExpr> &expr);
    void exprTypeChecker(const std::unique_ptr<Node::GenericExpr> &expr, std::shared_ptr<ConstituentToken> &ptr);
    ConstituentToken inferTypeNumericLiteral(const std::string &value);
    void setTypeOrThrow(std::shared_ptr<ConstituentToken> &ptr, ConstituentToken &next);
    private:
    std::shared_ptr<Node::AST> ast_;
    
};

#endif // SEMANTIC_HPP
