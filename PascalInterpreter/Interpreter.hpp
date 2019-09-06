/*
AST Parser Interpreter
*/


#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "Parser.hpp"
#include "Operator.hpp"


class NodeVisitor
{
public:
	NodeVisitor() : m_sfd(nullptr) {};
	virtual ~NodeVisitor() {};

	virtual void Reset() noexcept
	{
		m_sfd = nullptr;
	}

	void SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept
	{
		m_sfd = sfd;
	}

protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg);
	}

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void ErrorSFD(const std::string& msg, unsigned int pos)
	{
		throw MyExceptions::MsgExecption(msg, m_sfd, pos);
	}

protected:
	MyDebug::SrouceFileDebugger* m_sfd;
};


class Interpreter: public NodeVisitor
{
public:
	Interpreter()
		:
		m_pSymbolTable(nullptr),
		m_pMemoryTable(nullptr),
		m_pProcedureTable(nullptr)
	{}
	virtual ~Interpreter() {};
	
	virtual void Reset() noexcept override
	{
		for (auto& table : m_memoryTableVec)
			table.Reset();
		for (auto& table : m_symoblTableVec)
			table.Reset();
		m_memoryTableVec.clear();
		m_symoblTableVec.clear();
		m_scopeCounter = 0;
		m_pMemoryTable = nullptr;
		m_pSymbolTable = nullptr;
		m_sfd = nullptr;	
	}

	void PrintCurrentSymbolTable() noexcept
	{
		m_pSymbolTable->PrintTable();
	}

	void PrintCurrentMemoryTable() noexcept
	{
		m_pMemoryTable->PrintTable();
	}

	void PrintCurrentProcedureTable() noexcept
	{
		m_pProcedureTable->PrintTable();
	}

	void PrintAllSymbolTable() noexcept
	{
		for (auto& table : m_symoblTableVec)
			table.PrintTable();
	}

	void PrintAllMemoryTable() noexcept
	{
		for (auto& table : m_memoryTableVec)
			table.PrintTable();
	}

	void PrintAllProcedureTable() noexcept
	{
		for (auto& table : m_procedureTableVec)
			table.PrintTable();
	}

public:
	/*
	Functionality: interpreting the AST
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgram(SHARE_AST root)
	{
		return InterpretProgramEntryHelper(root);
	}

protected:
	inline void UpdateCurrentMemoryTable(ScopedMemoryTable* pMemoryTable)
	{
		m_pMemoryTable = pMemoryTable;
	}

	inline void UpdateCurrentSymbolTable(ScopedSymbolTable* pSymbolTable)
	{
		m_pSymbolTable = pSymbolTable;
	}

	inline void UpdateCurrentProcedureTable(ScopedProcedureTable* pProcedureTable)
	{
		m_pProcedureTable = pProcedureTable;
	}

	void AddMemoryTable(std::string name, unsigned int level)
	{
		if (m_memoryTableVec.empty())
		{
			if (level != 1)
				Error("MemoryError(Interpreter): adding a non-global(level!=1) table to a empty table vector.");
		}
		else if (m_memoryTableVec.back().GetLevel() != (level-1))
		{
			Error("MemoryError(Interpreter): adding a "+ MyTemplates::Str(level) +" table to a " + MyTemplates::Str(m_memoryTableVec.back().GetLevel()) + " table.");
		}
		m_memoryTableVec.push_back(ScopedMemoryTable(name, level));
		UpdateCurrentMemoryTable(&(m_memoryTableVec.back()));
	}

	void AddSymbolTable(std::string name, unsigned int level)
	{
		if (m_symoblTableVec.empty())
		{
			if (level != 1)
				Error("SymbolError(Interpreter): adding a non-global(level!=1) table to a empty table vector.");
		}
		else if (m_symoblTableVec.back().GetLevel() != (level - 1))
		{
			Error("SymbolError(Interpreter): adding a " + MyTemplates::Str(level) + " table to a " + MyTemplates::Str(m_symoblTableVec.back().GetLevel()) + " table.");
		}
		m_symoblTableVec.push_back(ScopedSymbolTable(name, level));
		UpdateCurrentSymbolTable(&(m_symoblTableVec.back()));
	}

	void AddProcedureTable(std::string name, unsigned int level)
	{
		if (m_procedureTableVec.empty())
		{
			if (level != 1)
				Error("SymbolError(Interpreter): adding a non-global(level!=1) table to a empty table vector.");
		}
		else if (m_procedureTableVec.back().GetLevel() != (level - 1))
		{
			Error("SymbolError(Interpreter): adding a " + MyTemplates::Str(level) + " table to a " + MyTemplates::Str(m_procedureTableVec.back().GetLevel()) + " table.");
		}
		m_procedureTableVec.push_back(ScopedProcedureTable(name, level));
		UpdateCurrentProcedureTable(&(m_procedureTableVec.back()));
	}

	void AddTable(std::string name, unsigned int level = 0)
	{
		m_scopeCounter++;
		if (level != 0 && m_scopeCounter != level)
			Error("TableError(Interpreter): try to add scope level " + MyTemplates::Str(level) + \
				" but the last level is " + MyTemplates::Str(m_scopeCounter) + " .");
		AddMemoryTable(name, m_scopeCounter);
		AddSymbolTable(name, m_scopeCounter);
		AddProcedureTable(name, m_scopeCounter);
	}

	void PopBackTable()
	{
		if ((m_memoryTableVec.size() != m_symoblTableVec.size()) \
			|| (m_memoryTableVec.back().GetLevel() != m_symoblTableVec.back().GetLevel()))
		{
			Error("TableError(Interpreter): memory table vector and symbol table vector has different length.");
		}
		else
		{
			if (m_memoryTableVec.back().GetLevel() != m_scopeCounter)
				Error("TableError(Interpreter): try to pop scope level " + MyTemplates::Str(m_scopeCounter) + \
					" but the last level is " + MyTemplates::Str(m_memoryTableVec.back().GetLevel()) +" .");

			if (m_scopeCounter > 1)
			{
				m_memoryTableVec.pop_back();
				m_symoblTableVec.pop_back();
				m_procedureTableVec.pop_back();
				m_scopeCounter--;
				UpdateCurrentMemoryTable(&(m_memoryTableVec.back()));
				UpdateCurrentSymbolTable(&(m_symoblTableVec.back()));
				UpdateCurrentProcedureTable(&(m_procedureTableVec.back()));
			}
		}
	}

	// Define a variable in symbol table
	void SymbolTableDefine(SHARE_VARDECL_AST varDecal)
	{
		std::string type = varDecal->GetTypeString();
		std::string name = varDecal->GetVarString();
		if (!m_pSymbolTable->define(name, VarSymbol(name, TypeSymbol(type))))
			ErrorSFD("SyntaxError(Interpreter): variable declaration already exists.", varDecal->GetVar()->GetToken()->GetPos());
	}

	// Check existence of a variable
	unsigned int SymbolTableLookUp(std::string name, MEMORY var)
	{
		for (unsigned int i = m_scopeCounter; i >= 1; i--)
		{
			if (!m_symoblTableVec[i-1].valid(m_symoblTableVec[i-1].lookup(name)))
				continue;
			else
				return i;
		}
		ErrorSFD("SymbolError(Interpreter): variable " + name + " is an undeclared variable.", var->GetPos());
	}

	// Check existence of a variable and it has a matched type
	unsigned int SymbolTableCheck(std::string name, MEMORY targetVar)
	{
		for (unsigned int i = m_scopeCounter; i >= 1; i--)
		{
			if (!m_symoblTableVec[i-1].check(name, targetVar))
				continue;
			else
				return i;
		}
		ErrorSFD("SymbolError(Interpreter): variable " + name + " with type " + m_pSymbolTable->lookup(name).GetType() + " does not match " + *(targetVar->GetValue()) + " with type " + targetVar->GetType() + " .", targetVar->GetPos());
	}

	// Define a variable in memory
	void MemoryTableDefine(std::string name, MEMORY var, unsigned int scopeCounter = 0)
	{
		if (scopeCounter == 0)
		{
			m_pMemoryTable->define(name, var);
		}
		else
		{
			m_memoryTableVec[scopeCounter - 1].define(name, var);
		}
	}

	// Return the memory a variable has assigned to
	MEMORY MemoryTableLookUp(std::string name, MEMORY var, unsigned int scopeCounter=0)
	{
		// The scope of this varible is not specified, treated as a variable in the current scope
		// Thus, it is SymbolTableLookUp's responsibility to provide the correct scope for this variable to look up in the memory table.
		if (scopeCounter == 0)
		{
			auto memory = m_pMemoryTable->lookup(name);
			if (!m_pMemoryTable->valid(memory))
			{
				ErrorSFD("SymbolError(Interpreter): variable " + name + " used before reference.", var->GetPos());
				return MAKE_EMPTY_MEMORY;
			}
			else
				return memory;
		}
		else
		{
			auto memory = m_memoryTableVec[scopeCounter-1].lookup(name);
			if (!m_memoryTableVec[scopeCounter - 1].valid(memory))
			{
				ErrorSFD("SymbolError(Interpreter): variable " + name + " used before reference.", var->GetPos());
				return MAKE_EMPTY_MEMORY;
			}
			else
				return memory;
		}
	}

	// Define a variable in procedure table
	void ProcedureTableDefine(SHARE_PROCEDURE_AST var)
	{
		m_pProcedureTable->define(var->GetName(), var);
	}

	// Return the procedure AST a procedure name has assigned to
	SHARE_PROCEDURE_AST ProcedureTableLookUp(std::string name, MEMORY var)
	{
		auto memory = m_pProcedureTable->lookup(name);
		if (!m_pProcedureTable->valid(memory))
			ErrorSFD("SymbolError(Interpreter): procedure " + name + " used before reference.", var->GetPos());
		else
			return static_pointer_cast<Procedure_AST>(memory);
	}

protected:

	/*
	Functionality: interpreting the entry (PROGRAM, block, PROCEDURE, Declaration)
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgramEntryHelper(SHARE_AST root)
	{
		if (!root)
		{
			Error("ASTError(Interpreter): root of InterpretProgramEntryHelper is null.");
			return MAKE_EMPTY_MEMORY;
		}
			
		// Condition: is a program start
		if (SHARE_PROGRAM_AST root_0 = dynamic_pointer_cast<Program_AST>(root))
		{
			return VisitProgram(root_0);
		}
		// Condition: is a block right after the program start
		else if (SHARE_BLOCk_AST root_1 = dynamic_pointer_cast<Block_AST>(root))
		{
			return VisitBlock(root_1);
		}
		else
		{
			Error("SyntaxError(Interpreter): program entry not defined");
			return MAKE_EMPTY_MEMORY;
		}
	}

	/*
	Functionality: interpreting the program (statments, assignment, operators, variables)
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root)
	{
		if (!root)
		{
			Error("ASTError(Interpreter): root of InterpretProgramHelper is null.");
			return MAKE_EMPTY_MEMORY;
		}

		// Condition: is a compound statment
		if (SHARE_COMPOUND_AST root_0 = dynamic_pointer_cast<Compound_AST>(root))
		{
			return VisitCompound(root_0);
		}
		// Condition: is a binary operation
		else if (SHARE_BINARY_AST root_1 = dynamic_pointer_cast<BinaryOp_AST>(root))
		{
			return VisitBinary(root_1);
		}
		// Condition: is a unary operation
		else if (SHARE_UNARY_AST root_2 = dynamic_pointer_cast<UnaryOp_AST>(root))
		{
			return VisitUnary(root_2);
		}
		// Condition: is a empty statement
		else if (SHARE_EMPTY_AST root_3 = dynamic_pointer_cast<Empty_AST>(root))
		{
			return VisitEmpty(root_3);
		}
		// Condition: is a assign statement
		else if (SHARE_ASSIGN_AST root_4 = dynamic_pointer_cast<Assign_AST>(root))
		{
			return VisitAssign(root_4);
		}
		// Condition: is a assign statement
		else if (SHARE_PROCEDURE_AST root_5 = dynamic_pointer_cast<Procedure_AST>(root))
		{
			return VisitProcedureCall(root_5);
		}
		// Condition: is a variable/static
		else
		{
			return VisitVairbale(root);
		}
	}

protected:
	virtual SHARE_TOKEN_STRING VisitProgram(SHARE_PROGRAM_AST root)
	{
		DEBUG_MSG("Running program---> " + root->GetName());
		AddTable(root->GetName(), 1);
		return InterpretProgramEntryHelper(root->GetBlock());
	}

	virtual SHARE_TOKEN_STRING VisitProcedureCall(SHARE_PROCEDURE_AST root)
	{
		auto procedure = ProcedureTableLookUp(root->GetName(), root->GetToken());

		if (SHARE_COMPOUND_AST params = dynamic_pointer_cast<Compound_AST>(root->GetParams()))
		{
			return VisitProcedure(procedure, params);
		}
		else
		{
			return VisitProcedure(procedure, nullptr);
		}
	}

	virtual SHARE_TOKEN_STRING VisitProcedure(SHARE_PROCEDURE_AST root, SHARE_COMPOUND_AST params = nullptr)
	{
		DEBUG_MSG("Running procedure---> " + root->GetName());
		AddTable(root->GetName());

		// Process parameters
		if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root->GetParams()))
		{

			if (params == nullptr)
			{
				Error("SyntaxError(Interpreter): Procedure parameters are declared without reference.");
			}
			// Define parameter
			for (SHARE_AST& decal : declaration->GetAllChildren())
			{
				// Condition: is a variable declaration
				if (SHARE_DECLCONTAINER_AST _declConatiner = dynamic_pointer_cast<DeclContainer_AST>(decal))
				{
					for (SHARE_AST& varDecal : _declConatiner->GetAllChildren())
					{
						if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(varDecal))
						{
							SymbolTableDefine(_varDecal);
						}
						else
						{
							Error("SyntaxError(Interpreter): unknown parameter declaration");
						}
					}
					DEBUG_RUN(PrintCurrentSymbolTable());
				}
				else
				{
					Error("SyntaxError(Interpreter): unknown declaration");
				}
			}
			// Assign parameter
			for (auto& child : params->GetAllChildren())
			{
				if (SHARE_ASSIGN_AST params_assign = dynamic_pointer_cast<Assign_AST>(child))
				{
					VisitAssign(params_assign);
				}
				else
				{
					Error("SyntaxError(Interpreter): unknown parameter assignment.");
				}
			}
		}
		else if (params != nullptr)
		{
			Error("SyntaxError(Interpreter): Too many arguments for procedure.");
		}
		else
		{
			DEBUG_MSG("Procedure has no parameter");
		}

		return InterpretProgramEntryHelper(root->GetBlock());
	}

	virtual SHARE_TOKEN_STRING VisitBlock(SHARE_BLOCk_AST root)
	{
		// Process declarations.
		if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root->GetDeclaration()))
		{
			for (SHARE_AST& decal : declaration->GetAllChildren())
			{
				// Condition: is a variable declaration
				if (SHARE_DECLCONTAINER_AST _declConatiner = dynamic_pointer_cast<DeclContainer_AST>(decal))
				{
					for (SHARE_AST& varDecal : _declConatiner->GetAllChildren())
					{
						if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(varDecal))
						{
							SymbolTableDefine(_varDecal);
						}
						else
						{
							Error("SyntaxError(Interpreter): unknown variable declaration");
						}
					}
					DEBUG_RUN(PrintCurrentSymbolTable());
				}
				// Condition: is a procedure start
				else if (SHARE_PROCEDURE_AST _procedure = dynamic_pointer_cast<Procedure_AST>(decal))
				{
					ProcedureTableDefine(_procedure);
					DEBUG_RUN(PrintCurrentProcedureTable());
				}
				else
				{
					Error("ASTError(Interpreter): unknown declaration");
				}
			}
		}
		else
		{
			DEBUG_MSG("Has no declaration.");
		}
		// Process the rest of the program.
		auto result = InterpretProgramHelper(root->GetCompound());
		PopBackTable();
		return result;
	}

	virtual SHARE_TOKEN_STRING VisitCompound(SHARE_COMPOUND_AST root)
	{
		SHARE_TOKEN_STRING result;
		for (auto& child : root->GetAllChildren())
		{
			DEBUG_MSG("Running statements list---> " + child->ToString());
			result = InterpretProgramHelper(child);
		}
		return result;
	}

	virtual SHARE_TOKEN_STRING VisitBinary(SHARE_BINARY_AST root)
	{
		SHARE_TOKEN_STRING left = InterpretProgramHelper(root->GetLeft());
		SHARE_TOKEN_STRING right = InterpretProgramHelper(root->GetRight());
		SHARE_TOKEN_STRING op = InterpretProgramHelper(root->GetOp());
		DEBUG_MSG("Running binary operation---> Left: " + left->ToString() + " Right: " + right->ToString() + " OP: " + op->ToString());
		return m_opeartor.exprBinaryDeciamlNumOp(left, right, op);
	}

	virtual SHARE_TOKEN_STRING VisitUnary(SHARE_UNARY_AST root)
	{
		SHARE_TOKEN_STRING result = InterpretProgramHelper(root->GetExpr());
		if (result->GetType() == INTEGER)
		{
			// e.g when "---+++1" as a input, the interpreter receives it as reversed by how the parser works.
			// so '1' is the first input, and all '+' signs are ignored by purpose
			// we then iterativly resolve all '-' sign to get the correct expression, which is '-1'
			DEBUG_MSG("Before Unary handle---> " + result->ToString());
			if (root->GetToken()->GetType() == MINUS)
			{
				if (result->GetValue()->front() == '-')
				{
					std::string s(1, result->GetValue()->back());
					result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s), result->GetPos());
				}
				else
				{
					std::string s(*(result->GetValue()));
					s.insert(0, "-");
					result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s), result->GetPos());
				}
			}
			DEBUG_MSG("After Unary handle---> " + result->ToString());
		}
		else
		{
			ErrorSFD("ASTError(Interpreter): " + root->ToString() + " on " + result->ToString() + " is not valid.", root->GetToken()->GetPos());
		}
		return result;
	}

	virtual SHARE_TOKEN_STRING VisitAssign(SHARE_ASSIGN_AST root)
	{
		std::string name = root->GetVarName();
		auto rhs = InterpretProgramHelper(root->GetRight());
		auto scope = SymbolTableLookUp(name, rhs);
		auto scope_ = SymbolTableCheck(name, rhs);

		if (scope != scope_)
		{
			ErrorSFD("SymbolError(Interpreter): variable " + name + " has been found in multiple scoped symbol table.", rhs->GetPos());
		}
		else
		{
			MemoryTableDefine(name, rhs, scope);
		}
		return MAKE_EMPTY_MEMORY;
	}

	virtual SHARE_TOKEN_STRING VisitVairbale(SHARE_AST root)
	{
		auto token = root->GetToken();
		std::string type = token->GetType();

		// is variable
		if (type == ID)
		{
			DEBUG_RUN(PrintCurrentMemoryTable());
			std::string name = *(token->GetValue());
			auto scope = SymbolTableLookUp(name, token);
			return MemoryTableLookUp(name, token, scope);
		}
		// is a type declaration
		else if (type == TYPE)
		{
			// TODO: change code below
			return root->GetToken();
		}
		// is static
		else
		{
			return root->GetToken();
		}
	}

	virtual SHARE_TOKEN_STRING VisitEmpty(SHARE_EMPTY_AST root)
	{
		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

	
protected:
	Operator m_opeartor;

	unsigned int m_scopeCounter = 0;
	// Data structure that stores scoped symbol/memory table
	// The ith table is enclosed by the (i-1)th table
	std::vector<ScopedMemoryTable> m_memoryTableVec;
	std::vector<ScopedSymbolTable> m_symoblTableVec;
	std::vector<ScopedProcedureTable> m_procedureTableVec;

	// Pointer to the current symbol/memory table
	ScopedMemoryTable* m_pMemoryTable;
	ScopedSymbolTable* m_pSymbolTable;
	ScopedProcedureTable* m_pProcedureTable;
};


class SemanticAnalyzer : public Interpreter
{
public:
	SemanticAnalyzer() 
	{
		Interpreter::Interpreter();
	}
	virtual ~SemanticAnalyzer() {};

protected:

	/*
	Functionality: interpreting the program (statments, assignment, operators, variables)
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root) override
	{
		if (!root)
			Error("ASTError(Interpreter): root of InterpretProgramHelper is null.");

		// Condition: is a compound statment
		if (SHARE_COMPOUND_AST root_0 = dynamic_pointer_cast<Compound_AST>(root))
		{
			return VisitCompound(root_0);
		}
		// Condition: is a binary operation
		else if (SHARE_BINARY_AST root_1 = dynamic_pointer_cast<BinaryOp_AST>(root))
		{
			return VisitBinary(root_1);
		}
		// Condition: is a empty statement
		else if (SHARE_EMPTY_AST root_3 = dynamic_pointer_cast<Empty_AST>(root))
		{
			return VisitEmpty(root_3);
		}
		// Condition: is a assign statement
		else if (SHARE_ASSIGN_AST root_4 = dynamic_pointer_cast<Assign_AST>(root))
		{
			return VisitAssign(root_4);
		}
		// Condition: is a variable/static
		else
		{
			return VisitVairbale(root);
		}
	}

	virtual SHARE_TOKEN_STRING VisitBlock(SHARE_BLOCk_AST root)
	{
		// Process declarations.
		if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root->GetDeclaration()))
		{
			for (SHARE_AST& decal : declaration->GetAllChildren())
			{
				// Condition: is a variable declaration
				if (SHARE_DECLCONTAINER_AST _declConatiner = dynamic_pointer_cast<DeclContainer_AST>(decal))
				{
					for (SHARE_AST& varDecal : _declConatiner->GetAllChildren())
					{
						if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(varDecal))
						{
							SymbolTableDefine(_varDecal);
						}
						else
						{
							Error("SyntaxError(Interpreter): unknown variable declaration");
						}
					}
					DEBUG_RUN(PrintCurrentSymbolTable());
				}
				// Condition: is a procedure start
				else if (SHARE_PROCEDURE_AST _procedure = dynamic_pointer_cast<Procedure_AST>(decal))
				{
					ProcedureTableDefine(_procedure);
					DEBUG_RUN(PrintCurrentProcedureTable());
					VisitProcedure(_procedure);
				}
				else
				{
					Error("ASTError(Interpreter): unknown declaration");
				}
			}
		}
		else
		{
			DEBUG_MSG("Has no declaration.");
		}
		// Process the rest of the program.
		auto result = InterpretProgramHelper(root->GetCompound());
		PopBackTable();
		return result;
	}

	virtual SHARE_TOKEN_STRING VisitProcedure(SHARE_PROCEDURE_AST root, SHARE_COMPOUND_AST params = nullptr) override
	{
		DEBUG_MSG("Running procedure---> " + root->GetName());
		AddTable(root->GetName());

		// Process parameters
		if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root->GetParams()))
		{
			// Define parameter
			for (SHARE_AST& decal : declaration->GetAllChildren())
			{
				// Condition: is a variable declaration
				if (SHARE_DECLCONTAINER_AST _declConatiner = dynamic_pointer_cast<DeclContainer_AST>(decal))
				{
					for (SHARE_AST& varDecal : _declConatiner->GetAllChildren())
					{
						if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(varDecal))
						{
							SymbolTableDefine(_varDecal);
						}
						else
						{
							Error("SyntaxError(Interpreter): unknown parameter declaration");
						}
					}
					DEBUG_RUN(PrintCurrentSymbolTable());
				}
				else
				{
					Error("SyntaxError(Interpreter): unknown declaration");
				}
			}
		}
		else
		{
			DEBUG_MSG("Procedure has no parameter");
		}

		return InterpretProgramEntryHelper(root->GetBlock());
	}

	virtual SHARE_TOKEN_STRING VisitBinary(SHARE_BINARY_AST root) override
	{
		SHARE_TOKEN_STRING left = InterpretProgramHelper(root->GetLeft());
		SHARE_TOKEN_STRING right = InterpretProgramHelper(root->GetRight());
		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

	virtual SHARE_TOKEN_STRING VisitAssign(SHARE_ASSIGN_AST root) override
	{
		std::string name = root->GetVarName();
		auto rhs = InterpretProgramHelper(root->GetRight());
		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

	virtual SHARE_TOKEN_STRING VisitVairbale(SHARE_AST root) override
	{
		auto token = root->GetToken();
		std::string type = token->GetType();

		// is variable
		if (type == ID)
		{
			std::string name = *(token->GetValue());
			auto scope = SymbolTableLookUp(name, token);
		}
		// is a type declaration
		else if (type == TYPE)
		{
			// TODO: change code below
			return root->GetToken();
		}

		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

};