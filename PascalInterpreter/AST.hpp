#pragma once

#include <string>
#include <sstream>
#include <vector>
#include "Token.hpp"
#include "MyMacros.hpp"


class AST
{
public:
	AST() {};
	virtual ~AST() noexcept {};
	explicit AST(SHARE_TOKEN_STRING token)
	{
		(check_is_shared_ptr(token)) ? m_token = token : 
			throw MyExceptions::InterpreterExecption("token passed to a AST constructor must be a shared_ptr type.");
	}
	virtual SHARE_TOKEN_STRING GetToken() const noexcept
	{
		return m_token;
	}
	virtual std::string ToString() const noexcept
	{
		return "AST: ( " + m_token->ToString()+ " ) ";
	}
private:
	SHARE_TOKEN_STRING m_token;
};

class Empty_Op : public AST
{
public:
	Empty_Op() : AST(MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"))) {};
	std::string ToString() const noexcept override
	{
		return "Empty_Op : () ";
	}
};

class UnaryOp_AST : public AST
{
public:
	explicit UnaryOp_AST(SHARE_TOKEN_STRING op, SHARE_AST expr)
	{
		(check_is_shared_ptr(op)) ? m_op = op :
			throw MyExceptions::InterpreterExecption("op passed to a UnaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(expr)) ? m_expr = expr :
			throw MyExceptions::InterpreterExecption("expr passed to a UnaryOp_AST constructor must be a shared_ptr type.");
	}
	~UnaryOp_AST() noexcept override {};

	SHARE_TOKEN_STRING GetToken() const noexcept override
	{
		return m_op;
	}
	SHARE_AST GetExpr() const noexcept
	{
		return m_expr;
	}
	std::string ToString() const noexcept override
	{
		return "Unary OP: ( " + m_op->ToString() + " , " + m_expr->ToString() + " ) ";
	}
private:
	SHARE_TOKEN_STRING m_op;
	SHARE_AST m_expr;
};

class BinaryOp_AST : public AST
{
public:
	explicit BinaryOp_AST(SHARE_AST left, SHARE_AST right, SHARE_AST op)
	{
		(check_is_shared_ptr(left)) ? m_left = left :
			throw MyExceptions::InterpreterExecption("left passed to a BinaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(right)) ? m_right = right :
			throw MyExceptions::InterpreterExecption("right passed to a BinaryOp_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(op)) ? m_op = op :
			throw MyExceptions::InterpreterExecption("op passed to a BinaryOp_AST constructor must be a shared_ptr type.");
	}
	~BinaryOp_AST() noexcept override {};
	SHARE_AST GetLeft() const noexcept
	{
		return m_left;
	}
	SHARE_AST GetRight() const noexcept
	{
		return m_right;
	}
	SHARE_AST GetOp() const noexcept
	{
		return m_op;
	}
	std::string ToString() const noexcept override
	{
		std::ostringstream oss;
		oss << "Binary OP: ( " << m_left->ToString() << " | " << m_right->ToString() << " | " << m_op->ToString() << " ) ";
		return oss.str();
	}
private:
	SHARE_AST m_left;
	SHARE_AST m_right;
	SHARE_AST m_op;
};

class Compound_AST : public AST
{
public:
	Compound_AST()
	{
	}
	void AddStatements(SHARE_AST child) noexcept
	{
		m_children.push_back(child);
	}
	std::vector<SHARE_AST> GetAllChildren() const noexcept
	{
		return m_children;
	}
	std::string ToString() const noexcept override
	{
		std::ostringstream oss;
		oss << "Compound_AST : ( ";
		for (auto& child : m_children)
		{
			oss << " { " << child->ToString() << " }, ";
		}
		oss << " ) ";
		return oss.str();
	}
private:
	std::vector<SHARE_AST> m_children;
};

class Assign_AST : public AST
{
public:
	explicit Assign_AST(SHARE_AST left, SHARE_AST right, SHARE_AST op)
	{
		(check_is_shared_ptr(left)) ? m_left = left :
			throw MyExceptions::InterpreterExecption("left passed to a Assign constructor must be a shared_ptr type.");
		(check_is_shared_ptr(right)) ? m_right = right :
			throw MyExceptions::InterpreterExecption("right passed to a Assign constructor must be a shared_ptr type.");
		(check_is_shared_ptr(op)) ? m_op = op :
			throw MyExceptions::InterpreterExecption("op passed to a Assign constructor must be a shared_ptr type.");
	}
	~Assign_AST() noexcept override {};

	std::string GetVarName() const noexcept
	{
		return m_left->GetToken()->ToString();
	}

	SHARE_AST GetLeft() const noexcept
	{
		return m_left;
	}
	SHARE_AST GetRight() const noexcept
	{
		return m_right;
	}
	SHARE_AST GetOp() const noexcept
	{
		return m_op;
	}
	std::string ToString() const noexcept override
	{
		std::ostringstream oss;
		oss << "Assign OP: ( " << m_left->ToString() << " | " << m_right->ToString() << " | " << m_op->ToString() << " ) ";
		return oss.str();
	}
private:
	SHARE_AST m_left;
	SHARE_AST m_right;
	SHARE_AST m_op;
};

class Program_AST : public AST
{
public:
	explicit Program_AST(SHARE_AST name, SHARE_AST block)
	{
		(check_is_shared_ptr(name)) ? m_name = name :
			throw MyExceptions::InterpreterExecption("op passed to a Program_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(block)) ? m_block = block :
			throw MyExceptions::InterpreterExecption("expr passed to a Program_AST constructor must be a shared_ptr type.");
	}
	~Program_AST() noexcept override {};

	std::string GetName() const noexcept
	{
		return *(m_name->GetToken()->GetValue());
	}

	SHARE_TOKEN_STRING GetToken() const noexcept override
	{
		return m_name->GetToken();
	}
	SHARE_AST GetBlock() const noexcept
	{
		return m_block;
	}
	std::string ToString() const noexcept override
	{
		return "Program: ( " + m_name->ToString() + " , " + m_block->ToString() + " ) ";
	}
private:
	SHARE_AST m_name;
	SHARE_AST m_block;
};

class Block_AST : public AST
{
public:
	explicit Block_AST(SHARE_AST declaration, SHARE_AST compound)
	{
		(check_is_shared_ptr(declaration)) ? m_declaration = declaration :
			throw MyExceptions::InterpreterExecption("op passed to a Block_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(compound)) ? m_compound = compound :
			throw MyExceptions::InterpreterExecption("expr passed to a Block_AST constructor must be a shared_ptr type.");
	}
	~Block_AST() noexcept override {};

	SHARE_AST GetDeclaration() const noexcept
	{
		return m_declaration;
	}
	SHARE_AST GetCompound() const noexcept
	{
		return m_compound;
	}
	std::string ToString() const noexcept override
	{
		return "Program: ( " + m_declaration->ToString() + " , " + m_compound->ToString() + " ) ";
	}
private:
	SHARE_AST m_declaration;
	SHARE_AST m_compound;
};

class Declaration_AST : public AST
{
public:
	Declaration_AST()
	{
	}
	void AddVarDecal(SHARE_AST child) noexcept
	{
		m_children.push_back(child);
	}
	std::vector<SHARE_AST> GetAllChildren() const noexcept
	{
		return m_children;
	}
	std::string ToString() const noexcept override
	{
		std::ostringstream oss;
		oss << "Declaration_AST : ( ";
		for (auto& child : m_children)
		{
			oss << " { " << child->ToString() << " }, ";
		}
		oss << " ) ";
		return oss.str();
	}
private:
	std::vector<SHARE_AST> m_children;
};

class DeclContainer_AST : public AST
{
public:
	DeclContainer_AST()
	{
	}
	void AddItem(SHARE_AST child) noexcept
	{
		m_children.push_back(child);
	}
	std::vector<SHARE_AST> GetAllChildren() const noexcept
	{
		return m_children;
	}
	std::string ToString() const noexcept override
	{
		std::ostringstream oss;
		oss << "DeclContainer_AST : ( ";
		for (auto& child : m_children)
		{
			oss << " { " << child->ToString() << " }, ";
		}
		oss << " ) ";
		return oss.str();
	}
private:
	std::vector<SHARE_AST> m_children;
};

class VarDecl_AST : public AST
{
public:
	explicit VarDecl_AST(SHARE_AST Var, SHARE_AST Type)
	{
		(check_is_shared_ptr(Var)) ? m_var = Var :
			throw MyExceptions::InterpreterExecption("op passed to a VarDecal_AST constructor must be a shared_ptr type.");
		(check_is_shared_ptr(Type)) ? m_type = Type :
			throw MyExceptions::InterpreterExecption("expr passed to a VarDecal_AST constructor must be a shared_ptr type.");
	}
	~VarDecl_AST() noexcept override {};

	SHARE_AST GetVar() const noexcept
	{
		return m_var;
	}
	SHARE_AST GetType() const noexcept
	{
		return m_type;
	}
	std::string ToString() const noexcept override
	{
		return "VarDecl_AST: ( " + m_var->ToString() + " , " + m_type->ToString() + " ) ";
	}
private:
	SHARE_AST m_var;
	SHARE_AST m_type;
};