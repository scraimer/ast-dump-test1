
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include <sstream>
#include <string>

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
	private:
		int _depth;
		int _count;
		bool _output_enabled;
		bool _found_logging;
		bool _found_endl_at_end;
  public:
   explicit FindNamedClassVisitor(ASTContext *Context)
       :  _depth(1), _count(0), _output_enabled(false), _found_logging(false), _found_endl_at_end(false), Context(Context)
   {
   }

	std::string indent(int level)
	{
		std::string s = "";
		for( int i=0; i<level; ++i )
		{
			s += "   ";
		}
		return s;
	}

	class dummy_ostream : public llvm::raw_ostream
	{
		private:
			virtual void write_impl( const char *, size_t) override
			{
			}
			virtual uint64_t current_pos() const override
			{
				return 1;
			}
	};

	llvm::raw_ostream& outs()
	{
		if( _output_enabled )
		{
			return llvm::outs() << indent(_depth);
		}
		else
		{
			static dummy_ostream dummy;
			return dummy;
		}
	}



   bool TraverseDecl(Decl *D)
   {
      // Write my own code here

      RecursiveASTVisitor<FindNamedClassVisitor>::TraverseDecl(D);  // Forward to base class
      return true;                                                  // Return false to stop the AST analyzing
   }

	bool is_of_class_for_MemberExpr(MemberExpr const * const e, std::string const & class_name)
	{
		ValueDecl const * decl = e->getMemberDecl();
		QualType qt = decl->getType();
		Type const * t = qt.getTypePtrOrNull();
		if( t )
		{
			RecordType const * rec = t->getAs<RecordType>();
			if( rec )
			{
				TagDecl const * tagdecl = rec->getDecl();
				IdentifierInfo * idinfo = tagdecl->getIdentifier();
				if( idinfo->getName() == class_name )
				{
					return true;
				}
			}
		}

		return false;
	}

	std::string get_qual_type_string(Expr const * const expr)
	{
		QualType expr_qualtype = expr->getType();
		SplitQualType expr_qt_split = expr_qualtype.split();
		return QualType::getAsString(expr_qt_split);
	}

	bool isLessLessOfILogLine(CXXOperatorCallExpr* const cxxoper_expr)
   {
      OverloadedOperatorKind oper_kind = cxxoper_expr->getOperator();
      if (oper_kind == OO_LessLess)
      {
         outs() << "       Operator Kind = '<<'\n";

         CallExpr* call_expr = static_cast<CallExpr*>(cxxoper_expr);
         int num_args = call_expr->getNumArgs();
         Expr** args = call_expr->getArgs();

         /*
         outs() << "       args num=" << num_args << "\n";
         for (int i = 0; i < num_args; ++i)
         {
            outs() << "       Arg[" << i << "] = " << get_qual_type_string(args[i]) << "\n";
         }
         */

			if( num_args >= 1 && get_qual_type_string(args[0]) == "class ILogLine")
			{
				return true;
			}
      }

		return false;
   }

	bool isLessLessOnLogEndl(CXXOperatorCallExpr* const cxxoper_expr)
   {
      OverloadedOperatorKind oper_kind = cxxoper_expr->getOperator();
      if (oper_kind == OO_LessLess)
      {
         outs() << "       Operator Kind = '<<'\n";

         CallExpr* call_expr = static_cast<CallExpr*>(cxxoper_expr);
         int num_args = call_expr->getNumArgs();
         Expr** args = call_expr->getArgs();

         /*
         outs() << "       args num=" << num_args << "\n";
         for (int i = 0; i < num_args; ++i)
         {
            outs() << "       Arg[" << i << "] = " << get_qual_type_string(args[i]) << "\n";
         }
         */

			if( num_args >= 2 && get_qual_type_string(args[1]) == "const class logendl")
			{
				return true;
			}
      }

		return false;
   }

	void reset_after_recusion_into_interesting()
	{
		_output_enabled = false;
		_found_logging = false;
		_found_endl_at_end = false;
	}



   bool TraverseStmt(Stmt *x)
   {
		std::string classname_logendl = "logendl";

		int id = ++_count;
		outs() << "BEGIN TraverseStmt [" << id << "]\n";
		bool found_interesting_statement_in_this_recursion = false;
	   bool found_first_lessless_of_ilogline = false;
      if (x != nullptr)
      {
	      auto stmt_class_name = x->getStmtClassName();
			outs() << "  Stmt ClassName='" << stmt_class_name << "'\n";

			// Try to figure out if it's an expression we can determine a type for
	      Expr* expr = nullptr;
			switch( x->getStmtClass() )
			{
				case Stmt::CXXOperatorCallExprClass:
					{
						_output_enabled = true;
						found_interesting_statement_in_this_recursion = true;
						outs() << "   Found Expr of interest!\n";

						CXXOperatorCallExpr* cxxoper_expr = reinterpret_cast<CXXOperatorCallExpr*>(x);
						expr = cxxoper_expr;
						if( cxxoper_expr)
						{
							outs() << "   QualType as string: " << get_qual_type_string(cxxoper_expr) << "\n";
							if( isLessLessOfILogLine( cxxoper_expr ) )
							{
								if( !_found_logging )
								{
									found_first_lessless_of_ilogline = true;
									_found_logging = true;
									outs() << "    Found logging!\n";
								}

                        if (found_first_lessless_of_ilogline && isLessLessOnLogEndl(cxxoper_expr))
                        {
                           _found_endl_at_end = true;
									outs() << "      Found endl at end!\n";
                        }
                     }
						}
               }
					break;
				default:
					outs() << "   Not interesting.\n";
					break;
			}

	      if( expr )
		   {
			   QualType expr_qualtype = expr->getType();
			   SplitQualType expr_qt_split = expr_qualtype.split();
				outs() << "     QualType as string: " << QualType::getAsString(expr_qt_split) << "\n";
			}

			std::ostringstream s;
			std::string cn = x->getStmtClassName();
         s << "Stmt of class: " << cn;

         FullSourceLoc FullLocation = Context->getFullLoc(x->getLocStart());
         if (FullLocation.isValid())
	      {
            s << " " << FullLocation.getSpellingLineNumber() << ":"
              << FullLocation.getSpellingColumnNumber();
         }
			outs() << s.str() << "\n";

			// Is this a DeclRefExpr to a 'class Logger *' ?
			if( cn == "DeclRefExpr" )
			{
				outs() << "   Found DeclRefExpr\n";
				DeclRefExpr* expr_decl = (DeclRefExpr*)x;

				DeclarationNameInfo nameinfo = expr_decl->getNameInfo();
				outs() << "    nameinfo: " << nameinfo.getAsString() << "\n";

				NamedDecl* ndecl = expr_decl->getFoundDecl();
            outs() << "    ndecl: " << ndecl->getNameAsString() << " getQualifiedNameAsString: " << ndecl->getQualifiedNameAsString() << "\n";
         }
			// Is this a member access to a member of type logendl?
			else if( cn == "MemberExpr" )
			{
				outs() << "Found MemberExpr\n";
				MemberExpr* member_expr = (MemberExpr*)x;
				if( is_of_class_for_MemberExpr( member_expr, classname_logendl ) )
				{
					outs() << "   It is an '" << classname_logendl << "'\n";
				}

				ValueDecl* decl = member_expr->getMemberDecl();
				outs() << "   decl: " << decl->getNameAsString() << "\n";

				QualType qt = decl->getType();
				Type const * t = qt.getTypePtrOrNull();
				if( t == NULL )
				{
					outs() << "     type = NULL\n";
				}
				else
				{
					char const * tcn = t->getTypeClassName();
					if( tcn )
					{
						outs() << "      type name=" << tcn << "\n";
					}
					else
					{
						outs() << "      type name=NULL\n";
					}

					RecordType const * rec = t->getAs<RecordType>();
					if( rec )
					{
						TagDecl const * tagdecl = rec->getDecl();
						IdentifierInfo * idinfo = tagdecl->getIdentifier();
						outs() << "               idinfo name=" << idinfo->getName() << "\n";
					}
					else
					{
						outs() << "       <<<<could not get as RecordType>>>>\n";
					}
				}
			}
      }
      else
	   {
			outs() << "Stmt:null\n";
		}

		outs() << "DIVE  TraverseStmt [" << id << "]\n";
		++_depth;
      RecursiveASTVisitor<FindNamedClassVisitor>::TraverseStmt(x);
      --_depth;
		outs() << "END   TraverseStmt [" << id << "]\n";

		if( found_interesting_statement_in_this_recursion )
		{
			outs() << "End of interest\n\n";

			if( _found_logging && !_found_endl_at_end )
			{
				if( outs().has_colors() )
				{
				outs().changeColor(raw_ostream::RED);
				}

				outs() << "ERROR: Found logging but no endl at the end!\n";

				if( outs().has_colors() )
				{
				outs().changeColor(raw_ostream::WHITE, false);
				}
			}

			reset_after_recusion_into_interesting();
		}

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
            outs() << "Found declaration at "
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

