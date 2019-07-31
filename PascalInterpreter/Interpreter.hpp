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
	Interpreter() {};
	virtual ~Interpreter() {};
	
	virtual void Reset() noexcept override
	{
		m_symbolTable.Reset();
		m_sfd = nullptr;
	}

	void PrintSymbolTable() noexcept
	{
		m_symbolTable.PrintTable();
	}

	void PrintMemoryTable() noexcept
	{
		GLOBAL_SCOPE.PrintTable();
	}

public:
	/*
	Functionality: interpreting the AST
	Return: InterpretProgram
	*/
	virtual void InterpretProgram(SHARE_AST root)
	{
		InterpretProgramEntryHelper(root);
	}

protected:

	inline void SymbolTableDefine(SHARE_VARDECL_AST varDecal)
	{
		std::string type = varDecal->GetTypeString();
		std::string name = varDecal->GetVarString();
		if (!m_symbolTable.define(name, VarSymbol(name, BuiltInTypeSymbol(type))))
			ErrorSFD("SyntaxError(Interpreter): variable declaration already exists.", varDecal->GetVar()->GetToken()->GetPos());
	}

	inline void SymbolTableLookUp(std::string name, SHARE_TOKEN_STRING token)
	{
		auto symbol = m_symbolTable.lookup(name);
		if (symbol.GetName() == "")
			ErrorSFD("SymbolError(Interpreter): variable " + name + " is an undeclared variable.", token->GetPos());
	}

	inline void SymbolTableCheck(std::string name, SHARE_TOKEN_STRING token)
	{
		if (!m_symbolTable.check(name, token->GetType()))
			ErrorSFD("SymbolError(Interpreter): variable " + name + " does not match type " + token->GetType() + " .", token->GetPos());
	}

	inline void MemoryTableDefine(std::string name, SHARE_TOKEN_STRING token)
	{
		GLOBAL_SCOPE.define(name, token);
	}

	inline SHARE_TOKEN_STRING MemoryTableLookUp(std::string name, SHARE_TOKEN_STRING token)
	{
		auto memory = GLOBAL_SCOPE.lookup(name);
		if (memory->GetType() == EMPTY)
			ErrorSFD("SymbolError(Interpreter): variable " + name + " used before reference.", token->GetPos());
		else
			return memory;
	}

protected:

	/*
	Functionality: interpreting the entry (PROGRAM, block, PROCEDURE, Declaration)
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgramEntryHelper(SHARE_AST root)
	{
		if (!root)
			Error("ASTError(Interpreter): root of InterpretProgramEntryHelper is null.");

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
		}
	}

	/*
	Functionality: interpreting the program (statments, assignment, operators, variables)
	Return: InterpretProgram
	*/
	virtual SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root)
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
		// Condition: is a variable/static
		else
		{
			return VisitVairbale(root);
		}
	}


	virtual SHARE_TOKEN_STRING VisitProgram(SHARE_PROGRAM_AST root)
	{
		DEBUG_MSG("Running program---> " + root->GetName());
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
					DEBUG_RUN(m_symbolTable.PrintTable());
				}
				// Condition: is a procedure start
				else if (SHARE_PROCEDURE_AST _procedure = dynamic_pointer_cast<Procedure_AST>(decal))
				{
					DEBUG_MSG("Running procedure---> " + _procedure->GetName());
					InterpretProgramEntryHelper(_procedure->GetBlock());
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
		return InterpretProgramHelper(root->GetCompound());
	}

	virtual SHARE_TOKEN_STRING VisitCompound(SHARE_COMPOUND_AST root)
	{
		for (auto& child : root->GetAllChildren())
		{
			DEBUG_MSG("Running statements list---> " + child->ToString());
			InterpretProgramHelper(child);
		}
		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
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
		SymbolTableLookUp(name, rhs);
		SymbolTableCheck(name, rhs);
		MemoryTableDefine(name, rhs);
		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
	}

	virtual SHARE_TOKEN_STRING VisitVairbale(SHARE_AST root)
	{
		auto token = root->GetToken();
		std::string type = token->GetType();

		// is variable
		if (type == ID)
		{
			DEBUG_RUN(GLOBAL_SCOPE.PrintTable());
			std::string name = *(token->GetValue());
			SymbolTableLookUp(name, token);
			return MemoryTableLookUp(name, token);
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
	MemoryTable GLOBAL_SCOPE;
	SymbolTable m_symbolTable;
	Operator m_opeartor;
};


class SemanticAnalyzer : public Interpreter
{
public:
	SemanticAnalyzer() {};
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
			SymbolTableLookUp(name, token);
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