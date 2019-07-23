#pragma once
#include <string>
#include <map>
#include "MyExceptions.hpp"

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
		std::cout << "SymbolMap contains:\n";
		for (auto it = m_symbol_map.begin(); it != m_symbol_map.end(); it++)
			std::cout << it->first << " => " << it->second.ToString() << '\n';
	}
	void define(VarSymbol varSymbol)
	{
		m_symbol_map[varSymbol.GetName()] = varSymbol;
	}
	VarSymbol lookup(std::string name)
	{
		if (m_symbol_map.find(name) != m_symbol_map.end())
			return m_symbol_map.at(name);
		else
			throw MyExceptions::InterpreterExecption("NameError(SymbolTable): variable "+ name + " does not exist.");
	}
	void check(std::string name, std::string type)
	{
		auto result = lookup(name);
		if (result.GetType() != type)
			throw MyExceptions::InterpreterExecption("NameError(SymbolTable): variable " + name + " has type " + result.GetType() \
				+ " does not match " + type + " .");
	}
private:
	SYMBOL_MAP m_symbol_map;
};