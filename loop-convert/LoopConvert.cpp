
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...");


class FindNamedClassVisitor
    : public RecursiveASTVisitor<FindNamedClassVisitor>
{
  public:
   explicit FindNamedClassVisitor(ASTContext *Context)
       : Context(Context)
   {
   }

   bool TraverseDecl(Decl *D)
   {
	   // Write my own code here

      RecursiveASTVisitor<FindNamedClassVisitor>::TraverseDecl(D);  // Forward to base class
      return true;                                    // Return false to stop the AST analyzing
   }
   bool TraverseStmt(Stmt *x)
   {
		//CompoundStmt* cs = x->getAsType<CompoundStmt>();
		//if( x->getStmtClass() == 
		llvm::outs() << "Stmt of class: " << x->getStmtClassName() << "\n";
      FullSourceLoc FullLocation = Context->getFullLoc(x->getLocStart());
      if (FullLocation.isValid())
         llvm::outs() << "Found declaration at "
                      << FullLocation.getSpellingLineNumber() << ":"
                      << FullLocation.getSpellingColumnNumber() << "\n";

      RecursiveASTVisitor<FindNamedClassVisitor>::TraverseStmt(x);
      return true;
   }
   bool TraverseType(QualType x)
   {
      // your logic here
      RecursiveASTVisitor<FindNamedClassVisitor>::TraverseType(x);
      return true;
   }

   /*
   bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
   {
      if (Declaration->getQualifiedNameAsString() == "n::m::C")
      {
         FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getLocStart());
         if (FullLocation.isValid())
            llvm::outs() << "Found declaration at "
                         << FullLocation.getSpellingLineNumber() << ":"
                         << FullLocation.getSpellingColumnNumber() << "\n";
      }
      return true;
   }
   */

  private:
   ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer
{
  public:
   explicit FindNamedClassConsumer(ASTContext *Context)
       : Visitor(Context)
   {
   }

   virtual void HandleTranslationUnit(clang::ASTContext &Context)
   {
      Visitor.TraverseDecl(Context.getTranslationUnitDecl());
   }

  private:
   FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction
{
  public:
   virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
       clang::CompilerInstance &Compiler, llvm::StringRef InFile)
   {
      return std::unique_ptr<clang::ASTConsumer>(
          new FindNamedClassConsumer(&Compiler.getASTContext()));
   }
};

int main(int argc, char const **argv)
{
   CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
   ClangTool Tool(OptionsParser.getCompilations(),
                  OptionsParser.getSourcePathList());

 	return Tool.run(newFrontendActionFactory<FindNamedClassAction>().get());
}
