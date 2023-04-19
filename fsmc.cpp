/***********************************************************************\
*
* Contents: Finit State Machine compiler
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <exception>
#include <regex>
#include <stdexcept>

#include "scanner.h"
#include "parser.hpp"
#include "ast.h"
#include "codegenerator.h"
#include "dotgenerator.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/


/***************************** Functions *******************************/

int main(int argc, const char *argv[])
{
  int exitCode = 0;
  try
  {
    // parse arguments
    std::string inputFilePath;
    std::string outputFilePath = "";
    std::string dotDirectoryPath = "";
    uint        stateStackSize = 0;
    std::string logFunction = "";
    bool        asserts = false;
    bool        debug = false;
    bool        dumpAST = false;
    int i = 1;
    while (i < argc)
    {
      std::string argument(argv[i]);
      if      ((argument == "-h") || (argument == "--help"))
      {
        fprintf(stdout, "Usage: %s [<options>] [<input file>]\n", argv[0]);
        fprintf(stdout, "\n");
        fprintf(stdout, "Options\n");
        fprintf(stdout, "-o|--output <file path>               output file\n");
        fprintf(stdout, "-d|--dot-directory <directory path>   .dot file directory\n");
        fprintf(stdout, "-n|--state-stack-size <n>             state stack size\n");
        fprintf(stdout, "-l|--log-function <log function>      log function to call on state change\n");
        fprintf(stdout, "-a|--asserts                          generate asserts\n");
        fprintf(stdout, "--debug                               debug output\n");
        fprintf(stdout, "--dump-ast                            dump abstract syntax tree\n");
        return 0;
      }
      else if ((argument == "-o") || (argument == "--output"))
      {
        if (i >= argc) throw std::runtime_error("missing parameter for option --output");
        outputFilePath = argv[i+1];
        i += 2;
      }
      else if ((argument == "-d") || (argument == "--dot"))
      {
        if (i >= argc) throw std::runtime_error("missing parameter for option --dot-directory");
        dotDirectoryPath = argv[i+1];
        i += 2;
      }
      else if ((argument == "-n") || (argument == "--state-stack-size"))
      {
        if (i >= argc) throw std::runtime_error("missing parameter for option --state-stack-size");
        stateStackSize = static_cast<uint>(std::atoi(argv[i+1]));
        i += 2;
      }
      else if ((argument == "-l") || (argument == "--log-function"))
      {
        if (i >= argc) throw std::runtime_error("missing parameter for option --log-function");
        logFunction = argv[i+1];
        i += 2;
      }
      else if ((argument == "-a") || (argument == "--asserts"))
      {
        asserts = true;
        i += 1;
      }
      else if (argument == "--debug")
      {
        debug = true;
        i += 1;
      }
      else if (argument == "--dump-ast")
      {
        dumpAST = true;
        i += 1;
      }
      else
      {
        inputFilePath = argv[i];
        i += 1;
      }
    }

    // open input file/stdin
    std::ifstream inputFile;
    std::istream  *input;
    if (!inputFilePath.empty())
    {
      inputFile = std::ifstream(inputFilePath);
      if (!inputFile.is_open())
      {
        throw std::runtime_error("cannot open file '" + inputFilePath + "'");
      }
      input = &inputFile;
    }
    else
    {
      input = &std::cin;
    }

    // open output file/stdout
    std::ofstream outputFile;
    std::ostream *output;
    if (!outputFilePath.empty())
    {
      outputFile = std::ofstream(outputFilePath);
      if (!outputFile.is_open())
      {
        throw std::runtime_error("cannot open file '" + outputFilePath + "'");
      }
      output = &outputFile;
    }
    else
    {
      output = &std::cout;
    }

    // scan for FSMs and compile
    uint  lineNb         = 0;
    bool  fsm            = false;
    uint  fsmIndent      = 0;
    uint  fsmStartLineNb = 0;
    while (!input->eof())
    {
      // scan for FSM
      const std::regex COMMENT("^(.*)\\s*//$");
      const std::regex FSM_START("^(\\s*)#fsm\\s+(.*)$");
      const std::regex FSM_END("^\\s*#end$");

      std::stringstream fsmSource;
      while (!input->eof())
      {
        std::smatch match;

        // get line
        std::string line;
        std::getline(*input, line);
        lineNb++;

        // strip comment
        std::string s;
        if (std::regex_match(line, match, COMMENT))
        {
          s = match[1].str();
        }
        else
        {
          s = line;
        }

        // match line
        if      (std::regex_match(s, match, FSM_START))
        {
          fsmSource << line << std::endl;
          fsm            = true;
          fsmIndent      = match[1].str().length();
          fsmStartLineNb = lineNb;
        }
        else if (std::regex_match(s, match, FSM_END))
        {
          fsmSource << line << std::endl;
          fsm = false;
          break;
        }
        else if (fsm)
        {
          fsmSource << line << std::endl;
        }
        else
        {
          if (!dumpAST)
          {
            *output << line << std::endl;
          }
        }
      }
      if (fsm)
      {
        throw std::runtime_error("end of FSM not found");
      }

      if (!fsmSource.str().empty())
      {
        // init scanner
        Scanner scanner;
        scanner.switch_streams(&fsmSource, nullptr);
        scanner.setLineNumber(fsmStartLineNb);

        // parse FSM
        AST ast(stateStackSize,asserts);
        Parser parser(!inputFilePath.empty() ? inputFilePath : "<stdin>", scanner, ast);
        if (debug) parser.set_debug_level(1);
        if (parser.parse() != 0)
        {
          throw std::runtime_error("parsing failed");
        }

        // validate
        ast.validateStates();

        if (!dumpAST)
        {
          // generate code
          CodeGenerator codeGenerator(*output, fsmIndent, 2, logFunction);
          codeGenerator.generate(ast);
          
          // generate .dot file
          if (!dotDirectoryPath.empty())
          {
            std::string filePath = dotDirectoryPath+"/"+ast.getFSMName()+".dot";
            std::ofstream output(filePath);
            if (!output.is_open())
            if (!outputFile.is_open())
            {
              throw std::runtime_error("cannot open file '" + filePath + "'");
            }

            DotGenerator dotGenerator(output);
            dotGenerator.generate(ast);

            output.close();
          }
        }
        else
        {
          ast.print();
        }
      }
    }

    // close files
    if (outputFile.is_open())
    {
      outputFile.close();
    }
    if (inputFile.is_open())
    {
      inputFile.close();
    }
  }
  catch (const std::runtime_error &error)
  {
    fprintf(stderr, "ERROR: %s!\n", error.what());
    exitCode = 1;
  }
  catch (...)
  {
    std::exception_ptr exception = std::current_exception();
    try
    {
      if (exception) std::rethrow_exception(exception);
    }
    catch(const std::exception &exception)
    {
      fprintf(stderr, "INTERNAL ERROR: unhandled exception '%s'!\n", exception.what());
    }
    exitCode = 127;
  }

  return exitCode;
}

/* end of file */
