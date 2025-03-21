#include "assembly.hpp"

#define HERE std::cout << "here: " << __LINE__ << std::endl

Compiler::Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename)
: filename_(filename)
{
  if (!ast) {
    throw std::invalid_argument("null ast");
  }
  ast_ = std::make_shared<Node::AST>(std::move(*ast));
  trunc();
  append();
  dispatch(ast_);
  initializeLocal();
}

void Compiler::initializeLocal()
{

  read();
  std::stringstream buffer;
  buffer << fs_.rdbuf();
  std::string content = buffer.str();
  open();
  fs_.seekp(0, std::ios::beg);
  fs_ << '\n' << "; ModuleID = '" << filename_ << "'\n";
  fs_ << "target triple = \"arm64-apple-macosx15.0.0\"" << "\n\n"; // for local system
  fs_ << "declare i32 @printf(ptr)\n\n";                           // enable logging
  append();
  fs_ << content;
  close();
}

void Compiler::dispatch(const std::shared_ptr<Node::AST> &tree)
{
  if (!tree || !tree->curr) {
    return; // end of AST
  }

  if (std::holds_alternative<Node::FunctionDecl>(*tree->curr)) {
    functionDeclaration(std::make_shared<Node::FunctionDecl>(
    std::get<Node::FunctionDecl>(std::move(*tree->curr))));
  }
  else if (std::holds_alternative<Node::GenericExpr>(*tree->curr)) {
    expression(std::make_unique<Node::GenericExpr>(
    std::move(std::get<Node::GenericExpr>(*tree->curr))));
  }

  if (tree->next) {
    dispatch(std::make_shared<Node::AST>(std::move(*tree->next))); // recursive call
  }
}

void Compiler::functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl)
{
  if (!decl) {
    throw std::invalid_argument("null function declaration");
  }

  switch (decl->type) {
    case ConstituentToken::FUNCTION_DECLARATION_F:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_VOID: {
      fs_ << "define i32 " << '@' << decl->token.value
          << declarationParameters(decl->parameters) << " {" << '\n'; // put params here
      functionBody(decl->body);
    } break;
    case ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA:
      break;
    default:
      throw std::invalid_argument("expected function type");
  }
  fs_ << "}\n\n";
}

/// @todo support for any return type (pass as param to this function)
std::string Compiler::declarationParameters(
const std::vector<std::pair<std::string, ConstituentToken>> &paramters)
{
  if (paramters.empty()) {
    return "()";
  }
  return ")(";
}

void Compiler::functionBody(const std::shared_ptr<Node::AST> &body)
{
  if (!body || !body->curr) {
    return;
  }
  if (std::holds_alternative<Node::ReturnStatement>(*body->curr)) {
    auto return_statement = std::get<Node::ReturnStatement>(std::move(*body->curr));
    if (return_statement.token.value == "ret") {
      fs_ << '\t' << "ret i32 0" << '\n';
    }
    else if (return_statement.token.value == "rev") {
      fs_ << '\t' << "ret i32 0" << '\n';
    }
  }
  if (std::holds_alternative<Node::GenericExpr>(*body->curr)) {
    expression(std::make_unique<Node::GenericExpr>(
    std::move(std::get<Node::GenericExpr>(*body->curr))));
  }

  if (body->next) {
    functionBody(std::make_shared<Node::AST>(std::move(*body->next)));
  }
}

void Compiler::functionCall(const std::unique_ptr<Node::FunctionCall> &call)
{
  if (!call) {
    return;
  }
  if (call->token.value == "print") {
    fs_ << "\tcall i32 @printf(ptr @.str)\n";
  }

  expression(call->parameters[0]);
}

void Compiler::expression(const std::unique_ptr<Node::GenericExpr> &call)
{
  if (std::holds_alternative<Node::FunctionCall>(*call->expr)) {
    functionCall(std::make_unique<Node::FunctionCall>(
    std::get<Node::FunctionCall>(std::move(*call->expr))));
  }
  else if (std::holds_alternative<Node::StringLiteral>(*call->expr)) {
    auto node = std::get<Node::StringLiteral>(*call->expr);
    stringGlobal(node.token.value);
  }
}
void Compiler::stringGlobal(const std::string &str)
{
  std::string formatted(str.begin() + 1, str.end() - 1);
  read();
  std::stringstream buffer;
  buffer << fs_.rdbuf();
  std::string content = buffer.str();
  open();
  fs_.seekp(0, std::ios::beg);
  fs_ << "@.str = private unnamed_addr constant [" << formatted.size() + 2 << " x i8] c\""
      << formatted << "\\0A\\00\", align 1\n\n";
  append();
  fs_ << content;
}

inline void Compiler::open()
{
  fs_.close();
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
  fs_.close();
  fs_.open(filename_, std::ios::app);
  if (!fs_) {
    throw std::invalid_argument("error: unable to append to output file");
  }
}

inline void Compiler::readWrite()
{
  fs_.close();
  fs_.open(filename_, std::ios::in | std::ios::out);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read/write to output file");
  }
}
inline void Compiler::read()
{
  fs_.close();
  fs_.open(filename_, std::ios::in);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read output file");
  }
}

inline void Compiler::trunc()
{
  fs_.close();
  fs_.open(filename_, std::ios::out | std::ios::trunc);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read output file");
  }
}