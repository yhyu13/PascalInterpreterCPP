#pragma once

/*
MACRO
*/
#include <iostream>
#include <memory>
#include "MyDebug.hpp"
using namespace std;

#define SHARE_STRING std::shared_ptr<std::string>
#define SHARE_TOKEN_STRING std::shared_ptr<Token<std::shared_ptr<std::string>>>
#define SHARE_AST std::shared_ptr<AST>
#define SHARE_UNARY_AST std::shared_ptr<UnaryOp_AST>
#define SHARE_BINARY_AST std::shared_ptr<BinaryOp_AST>
#define SHARE_COMPOUND_AST std::shared_ptr<Compound_AST>
#define SHARE_ASSIGN_AST std::shared_ptr<Assign_AST>
#define SHARE_EMPTY_AST std::shared_ptr<Empty_Op>

#define TOKEN_STRING_MAP std::map<std::string, SHARE_TOKEN_STRING>
#define TOKEN_STRING_PAIR std::pair<std::string, SHARE_TOKEN_STRING>

#define Myprintln(var) std::cout << var->ToString() << std::endl;

#define MAKE_SHARE_STRING(var) std::make_shared<std::string>(var)
#define MAKE_SHARE_TOKEN(type,var) std::make_shared<Token<decltype(var)>>(type,var)
#define MAKE_SHARE_AST(token) std::make_shared<AST>(token)
#define MAKE_SHARE_UNARY_AST(op, expr) std::make_shared<UnaryOp_AST>(op, expr)
#define MAKE_SHARE_BINARY_AST(left, right, op) std::make_shared<BinaryOp_AST>(left, right, op)
#define MAKE_SHARE_COMPOUND_AST() std::make_shared<Compound_AST>()
#define MAKE_SHARE_ASSIGN_AST(left, right, op) std::make_shared<Assign_AST>(left, right, op)
#define	MAKE_SHARE_EMPTY_AST() std::make_shared<Empty_Op>()

#define CREATE_SHARE_STRING(name, var) std::shared_ptr<std::string> name(new std::string(var));
#define CREATE_SHARE_TOKEN(name, type, var) std::shared_ptr<Token<std::shared_ptr<std::string>>> name(new Token<std::shared_ptr<std::string>>(type, var));
#define CREATE_SHARE_AST(name, token) std::shared_ptr<AST> name(new AST(token));
#define CREATE_SHARE_UNARY_AST(name, op, expr) std::shared_ptr<UnaryOp_AST> name(new UnaryOp_AST(op, expr));
#define CREATE_SHARE_BINARY_AST(name, left, right, op) std::shared_ptr<BinaryOp_AST> name(new BinaryOp_AST(left, right, op));
#define CREATE_SHARE_COMPOUND_AST(name) std::shared_ptr<Compound_AST> name(new Compound_AST());
#define CREATE_SHARE_ASSIGN_AST(name, left, right, op) std::shared_ptr<Assign_AST> name(new Assign_AST(left, right, op));
#define CREATE_SHARE_EMPTY_AST(name) std::shared_ptr<Empty_Op> name(new Empty_Op());