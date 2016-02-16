// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"


using namespace clang::tooling;
using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

auto LoggerMatcher =
	parenExpr(
			hasType(cxxRecordDecl(hasName("Logger"))))
	.bind("a_logger");

class LoggerPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const auto *logger_node = Result.Nodes.getNodeAs<clang::ParenExpr>("a_logger"))
    {
	    /*
	    // The expression has a type. Let's get it, and fetch from the type's declaration the name of the type.
	    // Keep in mind, the type might not have name.
       QualType t = logger_node->getType();
       if (t->isClassType())
       {
          CXXRecordDecl *r = t->getAsCXXRecordDecl();
          if (r)
          {
             llvm::outs() << "r->getName() = " << r->getName() << "\n";
             */
             logger_node->dump();
             /*
          }
       }
       */

		 
    }
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  LoggerPrinter Printer;
  MatchFinder Finder;
  Finder.addMatcher(LoggerMatcher, &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}

