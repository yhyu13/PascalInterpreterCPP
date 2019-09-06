#include "Interpreter.hpp"

/*
Functionality: interpreting the entry (PROGRAM, block, PROCEDURE, Declaration)
Return: InterpretProgram
*/

SHARE_TOKEN_STRING Interpreter::InterpretProgramEntryHelper(SHARE_AST root)
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

SHARE_TOKEN_STRING Interpreter::InterpretProgramHelper(SHARE_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitProgram(SHARE_PROGRAM_AST root)
{
	DEBUG_MSG("Running program---> " + root->GetName());
	AddTable(root->GetName(), 1);
	return InterpretProgramEntryHelper(root->GetBlock());
}

SHARE_TOKEN_STRING Interpreter::VisitProcedureCall(SHARE_PROCEDURE_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitProcedure(SHARE_PROCEDURE_AST root, SHARE_COMPOUND_AST params)
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

SHARE_TOKEN_STRING Interpreter::VisitBlock(SHARE_BLOCk_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitCompound(SHARE_COMPOUND_AST root)
{
	SHARE_TOKEN_STRING result;
	for (auto& child : root->GetAllChildren())
	{
		DEBUG_MSG("Running statements list---> " + child->ToString());
		result = InterpretProgramHelper(child);
	}
	return result;
}

SHARE_TOKEN_STRING Interpreter::VisitBinary(SHARE_BINARY_AST root)
{
	SHARE_TOKEN_STRING left = InterpretProgramHelper(root->GetLeft());
	SHARE_TOKEN_STRING right = InterpretProgramHelper(root->GetRight());
	SHARE_TOKEN_STRING op = InterpretProgramHelper(root->GetOp());
	DEBUG_MSG("Running binary operation---> Left: " + left->ToString() + " Right: " + right->ToString() + " OP: " + op->ToString());
	return m_opeartor.exprBinaryDeciamlNumOp(left, right, op);
}

SHARE_TOKEN_STRING Interpreter::VisitUnary(SHARE_UNARY_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitAssign(SHARE_ASSIGN_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitVairbale(SHARE_AST root)
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

SHARE_TOKEN_STRING Interpreter::VisitEmpty(SHARE_EMPTY_AST root)
{
	return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
}

/*
Functionality: interpreting the program (statments, assignment, operators, variables)
Return: InterpretProgram
*/

SHARE_TOKEN_STRING SemanticAnalyzer::InterpretProgramHelper(SHARE_AST root)
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

SHARE_TOKEN_STRING SemanticAnalyzer::VisitBlock(SHARE_BLOCk_AST root)
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

SHARE_TOKEN_STRING SemanticAnalyzer::VisitProcedure(SHARE_PROCEDURE_AST root, SHARE_COMPOUND_AST params)
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

SHARE_TOKEN_STRING SemanticAnalyzer::VisitBinary(SHARE_BINARY_AST root)
{
	SHARE_TOKEN_STRING left = InterpretProgramHelper(root->GetLeft());
	SHARE_TOKEN_STRING right = InterpretProgramHelper(root->GetRight());
	return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
}

SHARE_TOKEN_STRING SemanticAnalyzer::VisitAssign(SHARE_ASSIGN_AST root)
{
	std::string name = root->GetVarName();
	auto rhs = InterpretProgramHelper(root->GetRight());
	return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"), 0);
}

SHARE_TOKEN_STRING SemanticAnalyzer::VisitVairbale(SHARE_AST root)
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
