//------------------------------------------------------------------------------
// mutation strategy: replace variable
// inputs: first random ori var and random applicable rep var
// to do: consider type consistency
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
using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
static int vardeclNumber = 0;
static int tmpVarDeclNumber = 0;
static bool hasRandom = false;
static int randomVar = 0;

static int usedtorepVar = 0;
static string usedtorepVarName = "";

static bool isFirst = true;
static bool beginMutate = false;
static int tmpVarDeclNumberForMutate = 0;
//redefined each pass
static string currFunctionName = "";
static int currCompoundStmtNum = 0;
// static string mutateType = "";
// static string oldOp = "";
// static string newOp = "";
// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
static map<string,vector<int>> functionMapVar;
static map<string,vector<int>> funcCompoundStmtMapVar;

static map<int,string> varMapFunc;
static map<int,string> varMapFuncCompoundStmt;

static vector<int> varGlobalList;
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    currFunctionName = f->getNameInfo().getAsString();
    if (isFirst == true) {
        vector<int> tmplist;
        pair <string,vector<int>> tmpfunc = make_pair (currFunctionName,tmplist);  
        functionMapVar.insert(tmpfunc);
    } 
    return true;
  }

  bool VisitCompoundStmt(CompoundStmt *cs) {
    currCompoundStmtNum++;
    if (isFirst == true) {
        vector<int> tmplist;
        stringstream ss;
        string str;
        ss<<currCompoundStmtNum;
        ss>>str;
        pair <string,vector<int>> tmpfuncCompoundStmt = make_pair (currFunctionName+"_"+str,tmplist); 
        funcCompoundStmtMapVar.insert(tmpfuncCompoundStmt);
    }
    return true;
  }

  bool VisitVarDecl(VarDecl *d) {
    //global variables
    if (isFirst == true) {
        if (d->hasGlobalStorage() && !d->isLocalVarDeclOrParm()) {
            varGlobalList.push_back(TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL));
        }
        if (d->isLocalVarDeclOrParm()) {
            pair <int,string> tmpVarMapFunc = make_pair (TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL),currFunctionName);
            varMapFunc.insert(tmpVarMapFunc);
        }
        if (d->isLocalVarDeclOrParm() && !d->isLocalVarDecl()) {
            functionMapVar[currFunctionName].push_back(TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL));
        }
        if (d->isLocalVarDecl()) {
            stringstream ss;
            string str;
            ss<<currCompoundStmtNum;
            ss>>str;
            funcCompoundStmtMapVar[currFunctionName+"_"+str].push_back(TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL));

            pair <int,string> tmpVarMapFuncCompoundStmt = make_pair (TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL),currFunctionName+"_"+str);
            varMapFuncCompoundStmt.insert(tmpVarMapFuncCompoundStmt);
        }
    }
    if (beginMutate == true) {
        if (TheRewriter.getSourceMgr().getSpellingLineNumber(d->getLocStart(), NULL) == usedtorepVar) {
            usedtorepVarName = d->getQualifiedNameAsString();
            cout<<usedtorepVarName<<endl;
        }
    }
    return true;
  }

  bool VisitDeclRefExpr(DeclRefExpr *re) {
    if (isFirst == true){
        vardeclNumber++;
    }
    if (isFirst == false && beginMutate == false) {
        if(hasRandom == false){
            hasRandom = true;
            cout<<"total: "<<vardeclNumber<<endl;
            randomVar = (rand() % (vardeclNumber-1+1))+ 1;
            cout<<"randdom: "<<randomVar<<endl;
        }
        tmpVarDeclNumber++;
        if(tmpVarDeclNumber == randomVar){
            cout<<"mutate here"<<endl;
            cout<<re->getNameInfo().getAsString()<<" ";
            vector<int> usedtorepVars;

            int tmpDeclLine = TheRewriter.getSourceMgr().getSpellingLineNumber(re->getDecl()->getLocStart(), NULL);
            if(varMapFunc.count(tmpDeclLine)>0 && varMapFuncCompoundStmt.count(tmpDeclLine)>0){

                vector<int> thisFuncMapVars = functionMapVar[varMapFunc[tmpDeclLine]];
                for(int i =0; i<thisFuncMapVars.size(); i++){
                    cout<<thisFuncMapVars[i]<<" ";
                    usedtorepVars.push_back(thisFuncMapVars[i]);
                }

                vector<int> thisFuncCompoundStmtMapVars = funcCompoundStmtMapVar[varMapFuncCompoundStmt[tmpDeclLine]];
                for(int i =0; i<thisFuncCompoundStmtMapVars.size(); i++){
                    cout<<thisFuncCompoundStmtMapVars[i]<<" ";
                    usedtorepVars.push_back(thisFuncCompoundStmtMapVars[i]);
                }

                for(int i =0; i<varGlobalList.size(); i++){
                    cout<<varGlobalList[i]<<" ";
                    usedtorepVars.push_back(varGlobalList[i]);
                }
                cout<<endl;
            } else if (varMapFunc.count(tmpDeclLine)>0 && varMapFuncCompoundStmt.count(tmpDeclLine)==0){

                vector<int> thisFuncMapVars = functionMapVar[varMapFunc[tmpDeclLine]];
                for(int i =0; i<thisFuncMapVars.size(); i++){
                    cout<<thisFuncMapVars[i]<<" ";
                    usedtorepVars.push_back(thisFuncMapVars[i]);
                }

                for(int i =0; i<varGlobalList.size(); i++){
                    cout<<varGlobalList[i]<<" ";
                    usedtorepVars.push_back(varGlobalList[i]);
                }
                cout<<endl;
            } else if (varMapFunc.count(tmpDeclLine)==0 && varMapFuncCompoundStmt.count(tmpDeclLine)==0) {
                for(int i =0; i<varGlobalList.size(); i++){
                    cout<<varGlobalList[i]<<" ";
                    usedtorepVars.push_back(varGlobalList[i]);
                }
                cout<<endl;
            } else {
                cout<<"BUG!!!"<<endl;
            }
            usedtorepVar = usedtorepVars[(rand() % ((usedtorepVars.size()-1)+1))];
            cout<<"it uses: "<<usedtorepVar<<endl;
        }
    }
    if (beginMutate == true) {
        tmpVarDeclNumberForMutate++;
        if(tmpVarDeclNumberForMutate == randomVar){//usedtorepVarName
            TheRewriter.ReplaceText(re->getLocStart(), re->getNameInfo().getAsString().size(), usedtorepVarName); 
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
    currFunctionName="";
    currCompoundStmtNum=0;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    beginMutate = true;
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
  // newOp = argv[4];
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
