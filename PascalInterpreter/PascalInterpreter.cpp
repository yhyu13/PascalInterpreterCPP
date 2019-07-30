// InterpretorProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Visual Studio c++ memory leak
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "MonoHeader.hpp"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::string filename;
	std::string PWD = R"(C:\Users\yohan\source\repos\PascalInterpreter\)";

	
	while (true)
	{
		std::cout << PWD << "> ";
		if (std::getline(std::cin, filename))
		{
			if (filename != "")
			{
				try
				{


					//auto int_type = BuiltInTypeSymbol(INTEGER);
					//auto float_type = BuiltInTypeSymbol(FLOAT);

					//auto x = VarSymbol("x", int_type);
					//auto x2 = VarSymbol("x", float_type);
					//auto y = VarSymbol("y", float_type);

					//auto symtab = SymbolTable();
					//symtab.define(x);
					//symtab.PrintTable();
					//symtab.define(y);
					//symtab.PrintTable();
					//symtab.define(x2);
					//symtab.PrintTable();

					std::vector<std::string> src_file_vec;
					std::string src_file_oneliner;
					std::string LINE;
					std::ifstream infile(PWD + filename + ".txt");
					if (!infile)
					{
						std::cout << "'" << filename << "'" << " does not exist in the currenty working directory. Input a existed .txt file instead." << std::endl;
						continue;
					}

					while (getline(infile, LINE)) // To get you all the lines.
					{
						std::cout << LINE << std::endl; // Prints our STRING.
						src_file_oneliner += LINE + "\n";
						src_file_vec.push_back(LINE);
					}
					infile.close();

					auto sfd = MyDebug::SrouceFileDebugger(filename + ".txt", src_file_oneliner, src_file_vec);

					auto lexer = Lexer();
					lexer.Reset();
					lexer.SetText(sfd.GetOneliner());
					lexer.SetSFD(&sfd);

					auto parser = Parser();
					parser.Reset();
					parser.SetLexer(&lexer);
					parser.SetSFD(&sfd);
					auto root_tree = parser.GetProgramAST();

					auto inter = Interpreter();
					inter.Reset();
					inter.SetSFD(&sfd);
					inter.InterpretProgram(root_tree);
					inter.PrintVaribalesMap();
				}
				catch (const MyExceptions::MsgExecption& e)
				{
					std::cerr << e.what() << std::endl;
				}
				catch (const std::exception& e)
				{
					std::cerr << e.what() << std::endl;
				}
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}