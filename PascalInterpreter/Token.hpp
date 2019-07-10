#pragma once
#include <string>
#include <memory>
#include "MyTemplates.hpp"
#include "MyExceptions.hpp"

/*
Token types
*/
#define INTEGER "INTEGER"
#define FLOAT "FLOAT"
#define PLUS "PLUS"
#define MINUS "MINUS"
#define MUL "MUL"
#define DIV "DIV"
#define LEFT_PARATHESES "LEFT_PARATHESES"
#define RIGHT_PARATHESES "RIGHT_PARATHESES"
#define BEGIN "BEGIN"
#define END "END"
#define DOT "DOT"
#define ASSIGN "ASSIGN"
#define SEMI "SEMI"
#define ID "ID"
#define EMPTY "EMPTY"

#define __EOF__ "__EOF__"

enum NumOp_code
{
	eUNKNOWN = -1,
	ePLUS,
	eMINUS,
	eMULTIPLY,
	eDIVIDE
};

NumOp_code GetEnumNumOp(std::string const& op)
{
	if (op == PLUS) return ePLUS;
	else if (op == MINUS) return eMINUS;
	else if (op == MUL) return eMULTIPLY;
	else if (op == DIV) return eDIVIDE;
	else return eUNKNOWN;
}

/*
Token class
*/
template <typename T>
class Token
{
public:

	Token() : m_type("Undefined"), m_value(nullptr) {};
	explicit Token(std::string type, T value) 
	{
		m_type = type;
		(check_is_shared_ptr(value)) ? m_value = value : 
			throw MyExceptions::InterpreterExecption("Value passed to a Token constructor must be a shared_ptr type.");
	}
	virtual ~Token() {};

	std::string ToString() const
	{
		return "Token( " + m_type + ", " + MyTemplates::Str(*m_value) + " )";
	}
	std::string GetType() const
	{
		return m_type;
	}
	T GetValue() const
	{
		return m_value;
	}
	void SetType(std::string type)
	{
		m_type = type;
	}
	void SetValue(T value)
	{
		m_value = value;
	}

private:
	std::string m_type;
	T m_value;
};
