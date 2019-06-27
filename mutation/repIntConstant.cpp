//------------------------------------------------------------------------------
// mutation strategy: replace constant value
// inputs: changed operation
//------------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <map> 
#include <fstream>
#include <vector>
#include <utility>
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/LiteralSupport.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/APFloat.h"
using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
static int vardeclNumber = 0;
static int tmpVarDeclNumber = 0;
static bool hasRandom = false;
static int randomVar = 0;

// static int usedtorepVar = 0;
// static string usedtorepVarName = "";

static bool isFirst = true;
// static bool beginMutate = false;
// static int tmpVarDeclNumberForMutate = 0;
//redefined each pass
// static string currFunctionName = "";
// static int currCompoundStmtNum = 0;
// static string mutateType = "";
// static string oldOp = "";
static string newOp = "";
// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
// static map<string,vector<int>> functionMapVar;
// static map<string,vector<int>> funcCompoundStmtMapVar;

// static map<int,string> varMapFunc;
// static map<int,string> varMapFuncCompoundStmt;

// static vector<int> varGlobalList;
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitExpr(Expr *e) {
    if (isa<IntegerLiteral>(e)) {
        if(isFirst == true){
            vardeclNumber++;
        }
        if(isFirst == false){
            if(hasRandom == false){
                hasRandom = true;
                cout<<"total: "<<vardeclNumber<<endl;
                randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                cout<<"randdom: "<<randomVar<<endl;
            }
            //IntegerLiteral* il = cast<IntegerLiteral>(e);
            tmpVarDeclNumber++;
            if(tmpVarDeclNumber == randomVar){
                cout<<"mutate here"<<endl;
                IntegerLiteral* il = cast<IntegerLiteral>(e);
                string oldvalue = (il->getValue()).toString(10,true);
                if (strcmp(newOp.c_str(),"+1")==0){
                    string newvalue = (il->getValue()+1).toString(10,true);
                    TheRewriter.ReplaceText(il->getLocStart(), oldvalue.size(), newvalue);
                } else if (strcmp(newOp.c_str(),"-1")==0){
                    string newvalue = (il->getValue()-1).toString(10,true);
                    TheRewriter.ReplaceText(il->getLocStart(), oldvalue.size(), newvalue);
                } else if (strcmp(newOp.c_str(),"*-1")==0){
                    int newvalueint = atoi(oldvalue.c_str())*(-1);
                    stringstream ss;
                    string newvalue;
                    ss<<newvalueint;
                    ss>>newvalue;
                    newvalue = "(" + newvalue + ")";
                    TheRewriter.ReplaceText(il->getLocStart(), oldvalue.size(), newvalue);
                } else if (strcmp(newOp.c_str(),"*0")==0){
                    string newvalue = "0";
                    TheRewriter.ReplaceText(il->getLocStart(), oldvalue.size(), newvalue);
                }
            }
        }
    }
    return true;
  }

private:
  Rewriter &TheRewriter;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : Visitor(R) {}

  void HandleTranslationUnit(ASTContext &Context) {
    /* we can use ASTContext to get the TranslationUnitDecl, which is
       a single Decl that collectively represents the entire source file */
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    isFirst = false;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  MyASTVisitor Visitor;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  
  int CopyFile(char *SourceFile,char *NewFile) 
  {  
    ifstream in; 
    ofstream out;  
    in.open(SourceFile,ios::binary);//
    if(in.fail())//
    {     
        cout<<"Error 1: Fail to open the source file."<<endl;    
        in.close();    
        out.close();    
        return 0; 
    }  
    out.open(NewFile,ios::binary);//
    if(out.fail())//
    {     
        cout<<"Error 2: Fail to create the new file."<<endl;    
        out.close();    
        in.close();    
        return 0; 
    }  
    else//
    {     
        out<<in.rdbuf();    
        out.close();    
        in.close();    
        return 1; 
    } 
  }
  
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    CopyFile("main.c", "mainori.c");
    TheRewriter.overwriteChangedFiles();
    rename("main.c", "mainvar.c");
    rename("mainori.c", "main.c");
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  srand( (unsigned)time( NULL ) );
  // mutateType = argv[3];
  // oldOp = argv[3];
  newOp = argv[3];
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
