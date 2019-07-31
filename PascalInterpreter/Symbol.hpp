#pragma once
#include <string>
#include <map>

#include "Token.hpp"

class Symbol
{
public:
	Symbol() {};
	explicit Symbol(std::string type) : m_type(type) {};
	virtual ~Symbol() noexcept {};
	virtual std::string ToString() noexcept
	{
		return m_type;
	}
private:
	std::string m_type;
};

class BuiltInTypeSymbol : public Symbol
{
public:
	explicit BuiltInTypeSymbol(std::string type) : Symbol(type) {};
};


class VarSymbol
{
public:
	VarSymbol() {};
	explicit VarSymbol(std::string name, Symbol type) : m_name(name), m_type(type) {};
	virtual ~VarSymbol() noexcept {};
	virtual std::string ToString() noexcept
	{
		return " < " + m_name + " , " + m_type.ToString() + " > ";
	}
	virtual std::string GetName() noexcept
	{
		return m_name;
	}
	virtual std::string GetType() noexcept
	{
		return m_type.ToString();
	}
private:
	std::string m_name;
	Symbol m_type;
};

class SymbolTable
{
public:
	SymbolTable() {};
	virtual ~SymbolTable() noexcept {};

	void Reset() noexcept
	{
		m_symbol_map.clear();
	}
	void PrintTable() noexcept
	{
		std::cout << "Symbol table contents\n_____________________\n";
		for (auto it = m_symbol_map.begin(); it != m_symbol_map.end(); it++)
			std::cout << it->first << " => " << it->second.ToString() << std::endl;
	}
	bool define(std::string name, VarSymbol var)
	{
		auto size = m_symbol_map.size();
		m_symbol_map.insert(SYMBOL_PAIR(name, var));
		return (size + 1) == m_symbol_map.size();
	}
	VarSymbol lookup(std::string name)
	{
		if (m_symbol_map.find(name) != m_symbol_map.end())
			return m_symbol_map.at(name);
		else
			return VarSymbol("", Symbol());
	}
	bool check(std::string name, std::string type)
	{
		auto result = lookup(name);
		return result.GetType() == type;
	}
private:
	SYMBOL_MAP m_symbol_map;
};


class MemoryTable
{
public:
	MemoryTable() {};
	virtual ~MemoryTable() noexcept {};

	void Reset() noexcept
	{
		m_memory_map.clear();
	}
	void PrintTable() noexcept
	{
		std::cout << "Memory map contains--->\n{\n";
		for (auto it = m_memory_map.begin(); it != m_memory_map.end(); it++)
			std::cout << it->first << " => " << it->second->ToString() << '\n';
		std::cout << '}' << std::endl;
	}
	void define(std::string name, SHARE_TOKEN_STRING value)
	{
		m_memory_map[name] = value;
	}
	SHARE_TOKEN_STRING lookup(std::string name)
	{
		if (m_memory_map.find(name) != m_memory_map.end())
			return m_memory_map.at(name);
		else
			return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

private:
	TOKEN_STRING_MAP m_memory_map;
};