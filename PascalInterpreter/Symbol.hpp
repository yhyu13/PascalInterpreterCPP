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

class TypeSymbol : public Symbol
{
public:
	explicit TypeSymbol(std::string type) : Symbol(type) {};
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

class ScopedSymbolTable
{
public:
	ScopedSymbolTable(): m_scopeName(""),m_scopedLevel(0) {};
	explicit ScopedSymbolTable(std::string name, unsigned int level)
		:
		m_scopeName(name),
		m_scopedLevel(level)
	{}
	virtual ~ScopedSymbolTable() noexcept {};

	void SetNameAndLevel(std::string name, unsigned int level)
	{
		m_scopeName = name;
		m_scopedLevel = level;
	}
	std::string GetName() const noexcept
	{
		return m_scopeName;
	}
	unsigned int GetLevel() const noexcept
	{
		return m_scopedLevel;
	}
	void Reset() noexcept
	{
		m_scopeName = "";
		m_scopedLevel = 0;
		m_symbol_map.clear();
	}
	void PrintTable() noexcept
	{
		std::cout << ("Symbol table contents--->Name: " + m_scopeName + " Level: " + MyTemplates::Str(m_scopedLevel) + "\n_____________________\n");
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
	bool valid(VarSymbol var)
	{
		return var.GetName() != "";
	}
	bool check(std::string name, MEMORY token)
	{
		auto result = lookup(name);
		return result.GetType() == token->GetType();
	}
private:
	SYMBOL_MAP m_symbol_map;
	std::string m_scopeName;
	unsigned int m_scopedLevel;
};


class ScopedMemoryTable
{
public:
	ScopedMemoryTable(): m_scopeName(""), m_scopedLevel(0) {};
	explicit ScopedMemoryTable(std::string name, unsigned int level) 
		: 
		m_scopeName(name), 
		m_scopedLevel(level) 
	{}
	virtual ~ScopedMemoryTable() noexcept {};

	void SetNameAndLevel(std::string name, unsigned int level)
	{
		m_scopeName = name;
		m_scopedLevel = level;
	}
	std::string GetName() const noexcept
	{
		return m_scopeName;
	}
	unsigned int GetLevel() const noexcept
	{
		return m_scopedLevel;
	}
	void Reset() noexcept
	{
		m_scopeName = "";
		m_scopedLevel = 0;
		m_memory_map.clear();
	}
	void PrintTable() noexcept
	{
		std::cout << "Memory map contains--->Name: " + m_scopeName + " Level: " + MyTemplates::Str(m_scopedLevel) + "\n{\n";
		for (auto it = m_memory_map.begin(); it != m_memory_map.end(); it++)
			std::cout << it->first << " => " << it->second->ToString() << '\n';
		std::cout << '}' << std::endl;
	}
	void define(std::string name, MEMORY value)
	{
		m_memory_map[name] = value;
	}
	MEMORY lookup(std::string name)
	{
		if (m_memory_map.find(name) != m_memory_map.end())
			return m_memory_map.at(name);
		else
			return MAKE_EMPTY_MEMORY;
	}
	bool valid(MEMORY var)
	{
		return var->GetType() != EMPTY;
	}

private:
	MEMORY_MAP m_memory_map;
	std::string m_scopeName;
	unsigned int m_scopedLevel;
};