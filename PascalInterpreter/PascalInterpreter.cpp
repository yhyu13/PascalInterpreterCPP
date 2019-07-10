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
					std::string file;
					std::string LINE;
					std::ifstream infile(PWD + filename + ".txt");
					if (!infile)
					{
						std::cout <<"'"<< filename <<"'" <<" does not exist in the currenty working directory. Input a existed .txt file instead." << std::endl;
						continue;
					}

					while (getline(infile, LINE)) // To get you all the lines.
					{
						std::cout << LINE << std::endl; // Prints our STRING.
						file = file + " " + LINE;
					}
					infile.close();

					//auto parser = Parser(file);
					//auto ast = parser.GetProgramAST();
					//Myprintln(ast);
					auto inter = Interpreter();
					inter.Reset();
					inter.SetText(file);
					inter.InterpretProgram();
					inter.PrintVaribalesMap();
				}
				catch (const MyExceptions::InterpreterExecption& e)
				{
					DEBUG_MSG(e.what());
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