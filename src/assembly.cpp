#include "assembly.hpp"

#define HERE std::cout << "here: " << __LINE__ << std::endl

/// @todo create a stack implementation of the current symbol table
/// @todo evaluate expressions
/// @todo optimize expressions
/// @todo support floating point operations and support for unsigned operations
/// @todo support printing multiple types, including unsigned types
/// @todo support recursion
/// @todo support casting
/// @todo support storing floats properly
/// @optimize identical string literals
/// @todo remove pointer wrapper for expression struct
/// @todo support casting for unsigned numbers

Compiler::Compiler(const std::shared_ptr<Node::AST> &ast, const std::string &filename)
: ast_(ast), filename_(filename)
{
    if (!ast_) {
        throw std::invalid_argument("null ast");
    }
    curr_expr_type = std::nullopt;

    // trunc();
    initializeLocal();
    dispatch(ast_);
    open();
    fs_ << globals_buffer_.str() << '\n' << file_buffer_.str();
}

void Compiler::initializeLocal()
{

    globals_buffer_ << '\n' << "; ModuleID = '" << filename_ << "'\n";
    globals_buffer_ << "target triple = \"arm64-apple-macosx15.0.0\""
                    << "\n\n";                              // for local system
    globals_buffer_ << "declare i32 @printf(ptr, ...)\n\n"; // enable logging
}

void Compiler::dispatch(const std::shared_ptr<Node::AST> &tree)
{
    if (!tree || !tree->curr) {
        return; // end of AST
    }

    if (std::holds_alternative<Node::FunctionDecl>(*tree->curr)) {
        scoped_registers_.clear();
        identifiers_.clear();
        unsigned_registers_.clear();
        functionDeclaration(
        std::make_shared<Node::FunctionDecl>(std::get<Node::FunctionDecl>(*tree->curr)));
    }
    else if (std::holds_alternative<Node::GenericExpr>(*tree->curr)) {
        expression(
        std::make_shared<Node::GenericExpr>(std::get<Node::GenericExpr>(*tree->curr)));
    }

    if (tree->next) {
        dispatch(tree->next); // recursive call
    }
}

void Compiler::functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl)
{
    if (!decl) {
        throw std::invalid_argument("null function declaration");
    }

    std::string asm_define = TYPE_ASM.at(decl->return_type);
    asm_define.append(" @" + decl->token.value);
    function_table_.insert({decl->token.value, asm_define});

    switch (decl->type) {
        case ConstituentToken::FUNCTION_DECLARATION_F: {
            auto params = declarationParameters(decl->parameters);
            file_buffer_ << "define " << asm_define << params.first << " {"
                         << '\n'; // put params here
            auto statement = decl->body;

            for (const auto &allocation : params.second) {
                file_buffer_ << allocation;
            }

            while (statement) {
                if (std::holds_alternative<Node::VariableDecl>(*statement->curr)) {
                    allocateVariables(std::make_shared<Node::VariableDecl>(
                    std::get<Node::VariableDecl>(*statement->curr)));
                }
                statement = (statement->next);
            }
            functionBody(decl->body);
        } break;
        case ConstituentToken::FUNCTION_DECLARATION_F_VOID: {
            auto params = declarationParameters(decl->parameters);
            file_buffer_ << "define " << asm_define << params.first << " {" << '\n';
            auto statement = decl->body;
            for (const auto &allocation : params.second) {
                file_buffer_ << allocation;
            }

            while (statement) {
                if (std::holds_alternative<Node::VariableDecl>(*statement->curr)) {
                    allocateVariables(std::make_shared<Node::VariableDecl>(
                    std::get<Node::VariableDecl>(*statement->curr)));
                }
                statement = (statement->next);
            }
            functionBody(decl->body);
        } break;
        case ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL:
            break;
        case ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA:
            break;
        default:
            throw std::invalid_argument("expected function type");
    }

    file_buffer_ << appendable_buffer_.str();
    appendable_buffer_.str("");
    appendable_buffer_.clear();
    file_buffer_ << "}\n\n";
    num_registers_ = 0;
}

/// @todo support for any return type (pass as param to this function)
std::pair<std::string, std::vector<std::string>> Compiler::declarationParameters(
const std::vector<std::pair<std::string, ConstituentToken>> &parameters)
{
    std::vector<std::string> vect;

    if (parameters.empty()) {
        num_registers_ = 1;
        return std::make_pair("()", vect);
    }
    std::vector<Expression> exprs;
    std::string str = "(";
    for (const auto &[id, tok] : parameters) {
        std::string register_num = "%" + std::to_string(num_registers_++);
        str += TYPE_ASM.at(tok) + " noundef " + register_num + ", ";

        insertIfUnsigned(tok, register_num);

        std::optional<Storage> storage = makeStorage(tok);

        scoped_registers_.insert({register_num, TYPE_ASM.at(tok)});
        exprs.push_back(Expression(id, TYPE_ASM.at(tok), register_num, storage));
    }
    if (str.size() > 2) {
        str.pop_back();
        str.pop_back();
    }
    if (num_registers_ == 0) {
        num_registers_ = 1;
    }
    else {
        num_registers_++;
    }

    for (const auto &expr : exprs) {
        vect.push_back(allocateParameters(expr));
        appendable_buffer_ << '\t' << "store " << expr.type << ' ' << expr.register_number
                           << ", ptr %" << (num_registers_ - 1) << ", "
                           << STRING_TYPE_ALIGN.at(expr.type) << '\n';
    }

    str += ")";
    return std::make_pair(str, vect);
}

void Compiler::functionBody(const std::shared_ptr<Node::AST> &body)
{
    if (!body || !body->curr) {
        return;
    }
    if (std::holds_alternative<Node::ReturnStatement>(*body->curr)) {
        returnStatement(std::make_shared<Node::ReturnStatement>(
        std::get<Node::ReturnStatement>(*body->curr)));
    }
    else if (std::holds_alternative<Node::GenericExpr>(*body->curr)) {
        expression(
        std::make_shared<Node::GenericExpr>(std::get<Node::GenericExpr>(*body->curr)));
    }
    else if (std::holds_alternative<Node::VariableDecl>(*body->curr)) {
        variableDeclaration(
        std::make_shared<Node::VariableDecl>(std::get<Node::VariableDecl>(*body->curr)));
    }
    else if (std::holds_alternative<Node::ConditionalStatement>(*body->curr)) {
        auto branches = conditionalStatement(std::make_shared<Node::ConditionalStatement>(
        std::get<Node::ConditionalStatement>(*body->curr)));

        for (const auto &str : branches) {
            if (str == "\tbranch_to_last_register\n") {
                appendable_buffer_ << "\tbr label %" << (num_registers_) << "\n\n";
            }
            else {
                appendable_buffer_ << str;
            }
        }
        appendable_buffer_ << "\tbr label %" << (num_registers_) << "\n\n"
                           << num_registers_++ << ":\n";
    }

    if (body->next) {
        functionBody(body->next);
    }
}

std::optional<std::shared_ptr<Expression>>
Compiler::functionCall(const std::shared_ptr<Node::FunctionCall> &call)
{
    if (!call) {
        throw std::invalid_argument("null function call");
    }
    if (call->token.value == "print" || call->token.value == "println") {
        auto call_parameters = call->parameters;
        if (call_parameters.size() != 1) {
            throw std::invalid_argument("expected string literal in print statement");
        }

        auto expr = expression(call_parameters.at(0));
        std::string format_specifier;
        if (expr->type == "i32") {
            format_specifier = (expr->storage == Storage::UNSIGNED) ? "%u" : "%d";
        }
        else if (expr->type == "i64") {
            format_specifier = (expr->storage == Storage::UNSIGNED) ? "%lu" : "%li";
        }
        else if (expr->type == "i8") {
            format_specifier = (expr->storage == Storage::UNSIGNED) ? "%hhu" : "%c";
        }
        else if (expr->type == "i16") {
            format_specifier = (expr->storage == Storage::UNSIGNED) ? "%hu" : "%hd";
        }
        else if (expr->type == "float") {
            format_specifier = "%f";
            expr = typeCast(expr, "double");
        }
        else if (expr->type == "double") {
            format_specifier = "%lf";
        }

        if (call->token.value == "println") {
            format_specifier.append("\\n");
        }

        std::string global = "\"" + format_specifier + "\"";

        if (!expr->identifier.has_value()) {
            std::string format_string = stringGlobal(global);
            appendable_buffer_ << "\t%" << num_registers_++
                               << " = call i32 (ptr, ...) @printf(ptr " << format_string
                               << ", " << expr->type << ' ' << expr->register_number
                               << ")\n";
        }
        else if (search_string_global_.find(expr->identifier.value()) !=
                 search_string_global_.end()) {
            std::string search_term = expr->identifier.value();
            if (call->token.value == "println") {
                search_term.pop_back();
                search_term.append("\n\"");
            }
            std::string format_string = stringGlobal(search_term);
            appendable_buffer_ << "\t%" << num_registers_++ << " = call i32 @printf(ptr "
                               << format_string << ")\n";
        }
        else if (expr->identifier) {
            std::string format_string = stringGlobal(global);
            appendable_buffer_ << "\t%" << num_registers_++
                               << " = call i32 (ptr, ...) @printf(ptr " << format_string
                               << ", " << expr->type << ' ' << expr->register_number
                               << ")\n";
        }
        return std::nullopt;
    }

    std::unordered_map<std::string, std::string> typeCastMap = {
    {"typeCastInt", "i32"},  {"typeCastChar", "i8"},     {"typeCastShort", "i16"},
    {"typeCastLong", "i64"}, {"typeCastFloat", "float"}, {"typeCastDouble", "double"}};

    if (typeCastMap.find(call->token.value) != typeCastMap.end()) {
        if (call->parameters.size() != 1) {
            throw std::invalid_argument("expected object in type cast call");
        }
        auto expr = expression(call->parameters.at(0));
        return typeCast(expr, typeCastMap[call->token.value]);
    }

    if (call->type == ConstituentToken::TYPE_VOID) {
        std::vector<std::shared_ptr<Expression>> vect;
        for (const auto &expr : call->parameters) {
            vect.push_back(expression(expr));
        }
        appendable_buffer_ << "\tcall void @" << call->token.value << '(';
        std::string call_parameters;
        for (const auto &expr_struct : vect) {
            call_parameters +=
            expr_struct->type + " noundef " + expr_struct->register_number + ",";
        }
        if (!call_parameters.empty()) {
            call_parameters.pop_back();
        }
        appendable_buffer_ << call_parameters << ")\n";
        return std::nullopt;
    }
    std::vector<std::shared_ptr<Expression>> vect;
    for (const auto &expr : call->parameters) {
        vect.push_back(expression(expr));
    }
    std::string register_num = "%" + std::to_string(num_registers_++);
    appendable_buffer_ << '\t' << register_num << " = call " << TYPE_ASM.at(call->type)
                       << " @" << call->token.value << '(';
    std::string call_parameters;
    for (const auto &expr_struct : vect) {
        call_parameters +=
        expr_struct->type + " noundef " + expr_struct->register_number + ",";
    }
    if (!call_parameters.empty()) {
        call_parameters.pop_back();
    }
    appendable_buffer_ << call_parameters << ")\n";

    std::optional<Storage> storage = makeStorage(call->type);
    return std::make_shared<Expression>(
    Expression(std::nullopt, TYPE_ASM.at(call->type), register_num, storage));
}

void Compiler::returnStatement(
const ::std::shared_ptr<Node::ReturnStatement> &return_statement)
{
    if (!return_statement) {
        throw std::invalid_argument("null return statement");
    }

    ConstituentToken return_type_token = return_statement->type;
    std::string return_type_asm = TYPE_ASM.at(return_type_token);

    if (return_type_token == ConstituentToken::TYPE_MAIN) {

        appendable_buffer_ << '\t' << "ret " << return_type_asm << " 0" << '\n';
    }
    else if (return_statement->token.value == "ret") {
        auto expr = expression(return_statement->expr);
        appendable_buffer_ << '\t' << "ret " << return_type_asm << ' '
                           << expr->register_number << '\n';
    }
    else if (return_statement->token.value == "rev") {
        // return_type_asm can be hardcoded -> better it throws
        if (return_type_asm != "void") {
            throw std::invalid_argument("error: invalid return type for void function");
        }
        appendable_buffer_ << '\t' << "ret " << return_type_asm << '\n';
    }
}

void Compiler::allocateVariables(const std::shared_ptr<Node::VariableDecl> &decl)
{
    if (!decl) {
        throw std::invalid_argument("null variable declaration");
    }

    std::string type_asm = TYPE_ASM.at(decl->type);
    std::string alignment = ALIGN_ASM.at(decl->type);
    std::string register_num = "%" + std::to_string(num_registers_++);

    identifiers_.insert({decl->token.value, {register_num, type_asm}});
    scoped_registers_.insert({register_num, type_asm});
    insertIfUnsigned(decl->type, register_num);

    file_buffer_ << '\t' << register_num << " = alloca " << type_asm << ", " << alignment
                 << '\n';
}

std::string Compiler::allocateParameters(const Expression &expr)
{
    std::string type_asm = expr.type;
    std::string register_number = "%" + std::to_string(num_registers_++);
    std::string alignment = STRING_TYPE_ALIGN.at(expr.type);

    identifiers_.insert({expr.identifier.value(), {register_number, type_asm}});
    scoped_registers_.insert({register_number, type_asm});
    return std::string('\t' + register_number + " = alloca " + type_asm + ", " +
                       alignment + '\n');
}

std::shared_ptr<Expression>
Compiler::expression(const std::shared_ptr<Node::GenericExpr> &call)
{
    if (!call) {
        throw std::invalid_argument("expression is null");
    }

    if (std::holds_alternative<Node::GenericExpr>(*call->expr)) {
        throw std::invalid_argument("expressions cannot point to expressions");
    }
    else if (std::holds_alternative<Node::BinaryExpr>(*call->expr)) {
        auto expr = std::get<Node::BinaryExpr>(*call->expr);
        auto lhs = expression(expr.lhs);
        auto rhs = expression(expr.rhs);

        if (!curr_expr_type) {
            throw std::invalid_argument("expression type not recognized");
        }

        if (lhs->storage != rhs->storage) {
            throw std::invalid_argument(
            "cannot perform operator on objects of different type");
        }

        std::string type_asm = TYPE_ASM.at(curr_expr_type.value());
        std::string alignment = STRING_TYPE_ALIGN.at(type_asm);
        std::string register_num = "%" + std::to_string(num_registers_++);
        std::optional<Storage> preop_storage = lhs->storage;

        switch (expr.op) {
            case ConstituentToken::ARITHMETIC_ADD: {
                // <result> = add <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fadd";
                }
                else {
                    op = "add";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
            } break;
            case ConstituentToken::ARITHMETIC_SUB: {
                // <result> = sub <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fsub";
                }
                else {
                    op = "sub";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
            } break;
            case ConstituentToken::ARITHMETIC_MUL: {
                // <result> = mul <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fmul";
                }
                else {
                    op = "mul";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
            } break;
            case ConstituentToken::ARITHMETIC_DIV: {
                // <result> = sdiv <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fdiv";
                }
                else if (preop_storage == Storage::UNSIGNED) {
                    op = "udiv";
                }
                else {
                    op = "sdiv";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
            } break;
            case ConstituentToken::ARITHMETIC_MOD: {
                // <result> = srem <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "frem";
                }
                else if (preop_storage == Storage::UNSIGNED) {
                    op = "urem";
                }
                else {
                    op = "srem";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
            } break;
            case ConstituentToken::COMPARISON_EQ: {
                // <result> = icmp eq <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fcmp oeq";
                }
                else {
                    op = "icmp eq";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::COMPARISON_LT: {
                // <result> = icmp slt <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fcmp olt";
                }
                else {
                    op = "icmp slt";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::COMPARISON_GT: {
                // <result> = icmp sgt <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fcmp ogt";
                }
                else {
                    op = "imcp sgt";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::COMPARISON_LTE: {
                // <result> = icmp sle <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fcmp ole";
                }
                else {
                    op = "icmp sle";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::COMPARISON_GTE: {
                // <result> = icmp sge <ty> <op1>, <op2>
                std::string op;
                if (type_asm == "float" || type_asm == "double") {
                    op = "fcmp oge";
                }
                else {
                    op = "icmp sge";
                }
                appendable_buffer_ << '\t' << register_num << " = " << op << ' '
                                   << type_asm << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::LOGICAL_AND: {
                throw std::invalid_argument("logical AND not implemented");
            } break;
            case ConstituentToken::LOGICAL_OR: {
                throw std::invalid_argument("logical OR not implemented");
            } break;
            case ConstituentToken::LOGICAL_NOT: {
                throw std::invalid_argument("logical NOT not implemented");
            } break;
            case ConstituentToken::BITWISE_AND: {
                // <result> = and <ty> <op1>, <op2>
                appendable_buffer_ << '\t' << register_num << " = and " << type_asm << ' '
                                   << lhs->register_number << ", " << rhs->register_number
                                   << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::BITWISE_OR: {
                // <result> = or <ty> <op1>, <op2>
                appendable_buffer_ << '\t' << register_num << " = or " << type_asm << ' '
                                   << lhs->register_number << ", " << rhs->register_number
                                   << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::BITWISE_XOR: {
                // <result> = xor <ty> <op1>, <op2>
                appendable_buffer_ << '\t' << register_num << " = xor " << type_asm << ' '
                                   << lhs->register_number << ", " << rhs->register_number
                                   << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::BITWISE_SL: {
                // <result> = shl <ty> <op1>, <op2>
                appendable_buffer_ << '\t' << register_num << " = shl " << type_asm << ' '
                                   << lhs->register_number << ", " << rhs->register_number
                                   << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            case ConstituentToken::BITWISE_SR: {
                // <result> = ashr <ty> <op1>, <op2>
                appendable_buffer_ << '\t' << register_num << " = ashr " << type_asm
                                   << ' ' << lhs->register_number << ", "
                                   << rhs->register_number << '\n';
                curr_expr_type = ConstituentToken::TYPE_BOOL;
            } break;
            default:
                throw std::invalid_argument("unrecognized operator");
        }
        std::optional<Storage> storage = makeStorage(curr_expr_type);
        Expression expr_struct(std::nullopt, type_asm, register_num, storage);
        return std::make_shared<Expression>(expr_struct);
    }
    else if (std::holds_alternative<Node::UnaryExpr>(*call->expr)) {
    }
    else if (std::holds_alternative<Node::NumericLiteral>(*call->expr)) {
        auto literal = std::get<Node::NumericLiteral>(*call->expr);
        curr_expr_type = literal.type;

        std::string formatted;
        for (const auto &digit : literal.token.value) {
            if (std::isdigit(digit) || digit == '.') {
                formatted += digit;
            }
        }

        std::optional<Storage> storage = makeStorage(literal.type);
        Expression expr_struct(std::nullopt, TYPE_ASM.at(curr_expr_type.value()),
                               formatted, storage);
        return std::make_shared<Expression>(expr_struct);
    }
    else if (std::holds_alternative<Node::StringLiteral>(*call->expr)) {
        auto node = std::get<Node::StringLiteral>(*call->expr);
        curr_expr_type = ConstituentToken::TYPE_STRING; // verify
        Expression expr_struct(node.token.value, TYPE_ASM.at(curr_expr_type.value()),
                               stringGlobal(node.token.value), std::nullopt);
        return std::make_shared<Expression>(expr_struct);
    }
    else if (std::holds_alternative<Node::Identifier>(*call->expr)) {
        auto identifier = std::get<Node::Identifier>(*call->expr);
        std::optional<Storage> storage = makeStorage(identifier.type);
        Expression id_ptr(identifier.token.value,
                          identifiers_.at(identifier.token.value).second,
                          identifiers_.at(identifier.token.value).first, storage);

        // %4 = load i32, ptr %2, align
        std::string type_asm = identifiers_.at(identifier.token.value).second;

        if (storage.value() != Storage::UNSIGNED) {
            curr_expr_type =
            REVERSE_TYPE_ASM.at(identifiers_.at(identifier.token.value).second).first;
        }
        else {
            curr_expr_type =
            REVERSE_TYPE_ASM.at(identifiers_.at(identifier.token.value).second).second;
        }

        std::string alignment = STRING_TYPE_ALIGN.at(type_asm);
        std::string register_num = "%" + std::to_string(num_registers_++);
        appendable_buffer_ << '\t' << register_num << " = load " << type_asm << ", "
                           << "ptr" << ' ' << id_ptr.register_number << ", " << alignment
                           << '\n';

        scoped_registers_.insert({register_num, type_asm});

        storage = makeStorage(curr_expr_type);
        Expression expr_struct(identifier.token.value, type_asm, register_num, storage);
        return std::make_shared<Expression>(expr_struct);
    }
    else if (std::holds_alternative<Node::FunctionCall>(*call->expr)) {
        auto optional = functionCall(
        std::make_shared<Node::FunctionCall>(std::get<Node::FunctionCall>(*call->expr)));
        if (!optional) {
            return std::make_shared<Expression>(
            Expression(std::nullopt, "err", "optional", std::nullopt));
        }
        return optional.value();
    }
    else {
        throw std::invalid_argument("unrecognized expression variant");
    }

    return std::make_shared<Expression>(
    Expression(std::nullopt, "ERROR", "TODO", std::nullopt));
}

std::string Compiler::stringGlobal(const std::string &str)
{
    std::string formatted;
    for (size_t i = 1; i < str.size() - 1; ++i) {
        if (str[i] == '\\') {
            if (i + 1 < str.size() - 1) {
                switch (str[i + 1]) {
                    case 'n':
                        formatted += '\n';
                        break;
                    case 't':
                        formatted += '\t';
                        break;
                    case '\\':
                        formatted += '\\';
                        break;
                    case '"':
                        formatted += '\"';
                        break;
                    default:
                        formatted += str[i + 1];
                        break;
                }
                ++i; // Skip the escaped character
            }
        }
        else {
            formatted += str[i];
        }
    }
    if (search_string_global_.find(str) != search_string_global_.end()) {
        return search_string_global_.at(str);
    }
    std::string identifier = "@.str." + std::to_string(num_string_constants_++);

    search_string_global_.insert({str, identifier});

    globals_buffer_ << identifier << " = private unnamed_addr constant ["
                    << formatted.size() + 1 << " x i8] c\"";

    for (char c : formatted) {
        if (c == '\n') {
            globals_buffer_ << "\\0A";
        }
        else if (c == '\t') {
            globals_buffer_ << "\\09";
        }
        else if (c == '\\') {
            globals_buffer_ << "\\5C";
        }
        else if (c == '\"') {
            globals_buffer_ << "\\22";
        }
        else {
            globals_buffer_ << c;
        }
    }

    globals_buffer_ << "\\00\", align 1\n";

    return identifier;
}

void Compiler::variableDeclaration(const std::shared_ptr<Node::VariableDecl> &decl)
{
    if (!decl) {
        throw std::invalid_argument("null variable declaration");
    }

    std::string type_asm = TYPE_ASM.at(decl->type);
    std::string alignment = ALIGN_ASM.at(decl->type);

    std::string register_num = identifiers_.at(decl->token.value).first;

    /// @todo in semantic optimize expressions of just numeric values to store only
    /// literals

    auto expr = expression(decl->expr);

    if (!expr) {
        throw std::invalid_argument("null expression structure");
    }

    if (expr->type != type_asm) {
        expr = typeCast(expr, type_asm);
        curr_expr_type = decl->type;
    }

    std::string stored_in = "ptr";
    std::string value_or_reg = expr->register_number;

    if (type_asm == "float") {
        stored_in = "float*";
        if (expr->register_number.front() != '%' &&
            expr->register_number.front() != '@') {
            // hardcoded precision allows this to work
            std::stringstream ss;
            ss << std::scientific << std::setprecision(16)
               << std::stof(expr->register_number);
            value_or_reg = ss.str();
        }
    }
    else if (type_asm == "double") {
        stored_in = "double*";
        if (expr->register_number.front() != '%' &&
            expr->register_number.front() != '@') {
            std::stringstream ss;
            ss << std::scientific << std::stod(expr->register_number);
            value_or_reg = ss.str();
        }
    }
    else {
        stored_in = "ptr";
    }

    appendable_buffer_ << '\t' << "store " << type_asm << ' ' << value_or_reg << ", "
                       << stored_in << ' ' << register_num << ", " << alignment << '\n';
}

std::vector<std::string>
Compiler::conditionalStatement(const std::shared_ptr<Node::ConditionalStatement> &cond)
{
    std::stringstream temp_buffer;
    temp_buffer << appendable_buffer_.str();
    appendable_buffer_.str("");
    appendable_buffer_.clear();

    auto expr_struct = expression(cond->expr);

    std::vector<std::string> branches;
    branches.push_back(temp_buffer.str());
    branches.push_back(appendable_buffer_.str());

    appendable_buffer_.str("");
    appendable_buffer_.clear();

    // if (expr_struct->type != "i1") {
    //   std::cout << expr_struct->type << std::endl;
    //   throw std::invalid_argument("expected boolean");
    // }
    std::stringstream label_and_body;
    std::string left_label = "%" + std::to_string(num_registers_++);

    appendable_buffer_ << std::string(left_label.begin() + 1, left_label.end()) << ":\n";
    functionBody(cond->body);
    label_and_body << appendable_buffer_.str();
    std::string right_label = "%" + std::to_string(num_registers_++);

    appendable_buffer_.str("");
    appendable_buffer_.clear();
    std::vector<std::string> new_branches;
    if (cond->next) {
        if (cond->next->expr) {
            new_branches = conditionalStatement(cond->next);
        }
        else if (cond->next->body) {
            functionBody(cond->next->body);
            new_branches.push_back(appendable_buffer_.str());
            appendable_buffer_.str("");
            appendable_buffer_.clear();
        }
    }

    branches.push_back("\tbr i1 " + expr_struct->register_number + ", label " + left_label

                       + ", label " + right_label + "\n\n");

    branches.push_back(label_and_body.str());
    std::string line;
    std::string last_line;
    while (std::getline(label_and_body, line)) {
        last_line = line;
    }
    if (last_line.substr(0, 4) != "\tret" && last_line.substr(0, 4) != "\trev") {
        branches.push_back("\tbranch_to_last_register\n");
    }
    branches.push_back(std::string(right_label.begin() + 1, right_label.end()) + ":\n");

    branches.insert(branches.end(), new_branches.begin(), new_branches.end());
    return branches;
}

inline void Compiler::insertIfUnsigned(const ConstituentToken &token,
                                       const std::string &reg)
{
    if (token == ConstituentToken::TYPE_UINT || token == ConstituentToken::TYPE_UCHAR ||
        token == ConstituentToken::TYPE_ULONG || token == ConstituentToken::TYPE_USHORT) {
        unsigned_registers_.insert(reg);
    }
}

inline bool Compiler::isUnsigned(const ConstituentToken &token)
{
    return (
    token == ConstituentToken::TYPE_UINT || token == ConstituentToken::TYPE_UCHAR ||
    token == ConstituentToken::TYPE_ULONG || token == ConstituentToken::TYPE_USHORT);
}

inline std::optional<Storage>
Compiler::makeStorage(const std::optional<ConstituentToken> &token)
{
    if (!token.has_value()) {
        throw std::invalid_argument("token must have value to cast to storage class");
    }

    if (isUnsigned(token.value())) {
        return Storage::UNSIGNED;
    }

    try {
        return SIZE_PRECEDENCE.at(TYPE_ASM.at(token.value())).first;
    }
    catch (const std::out_of_range &e) {
        return std::nullopt;
    }
}

inline std::shared_ptr<Expression>
Compiler::typeCast(const std::shared_ptr<Expression> &expr, const std::string &cast)
{
    if (!expr) {
        throw std::invalid_argument("null type cast expression");
    }

    const std::string base = expr->type;

    if (cast == expr->type) {
        return expr;
    }

    std::string register_num = "%" + std::to_string(num_registers_++);

    auto base_pair = SIZE_PRECEDENCE.at(base);
    auto cast_pair = SIZE_PRECEDENCE.at(cast);
    Storage base_storage = base_pair.first;
    Storage cast_storage = cast_pair.first;
    int base_precedence = base_pair.second;
    int cast_precedence = cast_pair.second;

    auto cast_instruction = [&](const std::string &base_type,
                                const std::string &cast_type,
                                const std::string &instruction) {
        appendable_buffer_ << "\t" << register_num << " = " << instruction << " "
                           << base_type << " " << expr->register_number << " to "
                           << cast_type << '\n';
    };

    if (base_storage == Storage::FLOAT && cast_storage == Storage::FLOAT &&
        base_precedence < cast_precedence) {
        cast_instruction(base, cast, "fpext");
    }
    else if (base_storage == Storage::FLOAT && cast_storage == Storage::FLOAT &&
             base_precedence > cast_precedence) {
        cast_instruction(base, cast, "fptrunc");
    }
    else if (base_storage == Storage::FLOAT && cast_storage == Storage::INT) {
        cast_instruction(base, cast, "fptosi");
    }
    else if (base_storage == Storage::INT && cast_storage == Storage::FLOAT) {
        cast_instruction(base, cast, "sitofp");
    }
    else if (base_storage == Storage::INT && cast_storage == Storage::INT &&
             base_precedence < cast_precedence) {
        cast_instruction(base, cast, "sext");
    }
    else if (base_storage == Storage::INT && cast_storage == Storage::INT &&
             base_precedence > cast_precedence) {
        cast_instruction(base, cast, "trunc");
    }
    else {
        std::cerr << "base: '" << base << "' cast: '" << cast << '\'' << std::endl;
        throw std::invalid_argument("error: non-convertible type");
    }
    auto expr_struct = Expression(std::nullopt, cast, register_num, cast_storage);
    return std::make_shared<Expression>(expr_struct);
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
