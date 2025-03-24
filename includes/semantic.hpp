#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <iostream>
#include <memory>

#include "ast.hpp"
#include "dictionary.hpp"
#include "token.hpp"

// clang-format off
class Analyzer
{
    public:
    Analyzer(const std::shared_ptr<Node::AST> &ast);
    void dispatch(const std::shared_ptr<Node::AST> &ast);
    void varTypeSetter(const std::shared_ptr<Node::GenericExpr> &expr, const ConstituentToken &curr);
    void exprTypeSetter(const std::shared_ptr<Node::GenericExpr> &expr);
    void exprTypeChecker(const std::shared_ptr<Node::GenericExpr> &expr, std::shared_ptr<ConstituentToken> &ptr);
    ConstituentToken inferTypeNumericLiteral(const std::string &value);
    void setTypeOrThrow(std::shared_ptr<ConstituentToken> &ptr, const ConstituentToken &next);
    private:
    std::shared_ptr<Node::AST> ast_;
    
};

#endif // SEMANTIC_HPP
