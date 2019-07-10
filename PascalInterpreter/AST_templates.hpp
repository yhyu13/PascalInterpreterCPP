#pragma once

#include <string>
#include <sstream>
#include "Token.hpp"

template <typename T>
class AST
{
public:
	AST() {};
	virtual ~AST() {};
	explicit AST(T token)
	{
		(check_is_shared_ptr(token)) ? m_token = token :
			throw MyExceptions::InterpreterExecption("Token passed to a AST constructor must be a shared_ptr type.");
	}
	virtual T GetToken() const
	{
		return m_token;
	}
	virtual std::string ToString() const
	{
		return "AST: ( " + m_token->ToString() + " ) ";
	}
private:
	T m_token;
};

template <typename T, typename W>
class UnaryOp_AST : public AST<T>
{
public:
	explicit UnaryOp_AST(T op, W expr)
	{
		(check_is_shared_ptr(op)) ? m_op = op :
			throw MyExceptions::InterpreterExecption("op passed to a UnaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(expr)) ? m_expr = expr :
			throw MyExceptions::InterpreterExecption("expr passed to a UnaryOp_AST constructor must be a shared_ptr type.");
	}
	~UnaryOp_AST() override {};

	W GetExpr() const
	{
		return m_expr;
	}
	T GetToken() const override
	{
		return m_op;
	}
	std::string ToString() const override
	{
		return "Unary OP: ( " + m_op->ToString() + " , " + m_expr->ToString() + " ) ";
	}
private:
	T m_op;
	W m_expr;
};

template <typename T, typename W, typename M>
class BinaryOp_AST : public AST<T>
{
public:
	explicit BinaryOp_AST(T left, W right, M op)
	{
		(check_is_shared_ptr(left)) ? m_left = left :
			throw MyExceptions::InterpreterExecption("left passed to a UnaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(right)) ? m_right = right :
			throw MyExceptions::InterpreterExecption("right passed to a UnaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(op)) ? m_op = op :
			throw MyExceptions::InterpreterExecption("op passed to a UnaryOp_AST constructor must be a shared_ptr type.");
	}
	~BinaryOp_AST() override {};
	T GetLeft() const
	{
		return m_left;
	}
	W GetRight() const
	{
		return m_right;
	}
	M GetOp() const
	{
		return m_op;
	}
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << "Binary OP: ( " << m_left->ToString() << " | " << m_right->ToString() << " | " << m_op->ToString() << " ) ";
		return oss.str();
	}
private:
	T m_left;
	W m_right;
	M m_op;
};