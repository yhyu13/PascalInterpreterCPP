#pragma once
#include <string>
#include <memory>
#include "MyTemplates.hpp"
#include "MyExceptions.hpp"
#include "MyMacros.hpp"

/*
Token class
*/
template <typename T>
class Token
{
public:

	Token() : m_type("Undefined"), m_value(nullptr), m_pos(0) {};
	explicit Token(std::string type, T value) 
	{
		m_pos = 0;
		m_type = type;
		(check_is_shared_ptr(value)) ? m_value = value : 
			throw MyExceptions::MsgExecption("Value passed to a Token constructor must be a shared_ptr type.");
	}
	explicit Token(std::string type, T value, unsigned int pos)
	{
		m_pos = pos;
		m_type = type;
		(check_is_shared_ptr(value)) ? m_value = value :
			throw MyExceptions::MsgExecption("Value passed to a Token constructor must be a shared_ptr type.");
	}

	virtual ~Token() {};

	std::string ToString() const noexcept
	{
		return "Token( " + m_type + ", " + MyTemplates::Str(*m_value) + " )";
	}
	std::string GetType() const noexcept
	{
		return m_type;
	}
	T GetValue() const noexcept
	{
		return m_value;
	}
	unsigned int GetPos() const noexcept
	{
		return m_pos;
	}

private:
	std::string m_type;
	T m_value;
	unsigned int m_pos;
};
