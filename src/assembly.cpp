#include "assembly.hpp"

#define HERE std::cout << "here: " << __LINE__ << std::endl;

Compiler::Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename) : filename_(filename)
{
  if (!ast) {
    throw std::invalid_argument("null ast");
  }
  ast_ = std::make_shared<Node::AST>(std::move(*ast));
  initializeLocal();
  dispatch(ast_);
}

void Compiler::initializeLocal()
{
  open();
  fs_ << '\n' << "; ModuleID = '" << filename_ << "'\n";
  fs_ << "target triple = \"arm64-apple-macosx15.0.0\"" << "\n\n"; // for local system
  fs_ << "declare i32 @printf(ptr)\n\n";                           // enable logging
  close();
}

void Compiler::dispatch(const std::shared_ptr<Node::AST> &tree)
{
  if (!tree || !tree->curr) {
    HERE;
    return; // end of AST
  }

  if (std::holds_alternative<Node::FunctionDecl>(*tree->curr)) {
    functionDeclaration(std::make_shared<Node::FunctionDecl>(std::move(std::get<Node::FunctionDecl>(*tree->curr))));
  }
  if (tree->next) {
    dispatch(std::make_shared<Node::AST>(std::move(*tree->next))); // recursive call
  }
}

void Compiler::functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl)
{
  append();
  if (!decl) {
    throw std::invalid_argument("null function declaration");
  }

  switch (decl->type) {
    case ConstituentToken::FUNCTION_DECLARATION_F:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_VOID: {
      fs_ << "define i32 " << '@' << decl->token.value << declarationParameters(decl->parameters) << " {"
          << '\n'; // put params here
      functionBody(decl->body);
    } break;
    case ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA:
      break;
    default:
      throw std::invalid_argument("expected function type");
  }
  close();
}

/// @todo support for any return type (pass as param to this function)
std::string Compiler::declarationParameters(const std::vector<std::pair<std::string, ConstituentToken>> &paramters)
{
  if (paramters.empty()) {
    return "()";
  }
  return "()"; // add params
}

void Compiler::functionBody(const std::shared_ptr<Node::AST> &body)
{
  if (!body || !body->curr) {
    throw std::invalid_argument("null function body");
  }
  if (std::holds_alternative<Node::ReturnStatement>(*body->curr)) {
    auto return_statement = std::get<Node::ReturnStatement>(std::move(*body->curr));
    if (return_statement.token.value == "ret") {
      fs_ << '\t' << "ret i32 0" << '\n' << "}\n\n";
    }
    else if (return_statement.token.value == "rev") {
      fs_ << '\t' << "ret i32 0" << '\n' << "}\n\n";
    }
  }
}
inline void Compiler::open()
{
  fs_.open(filename_, std::ios::out);
  if (!fs_) {
    throw std::invalid_argument("error: unable to open output file");
  }
}

inline void Compiler::close()
{
  fs_.close();
}

inline void Compiler::append()
{
  fs_.open(filename_, std::ios::app);
  if (!fs_) {
    throw std::invalid_argument("error: unable to append to output file");
  }
}