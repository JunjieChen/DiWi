//------------------------------------------------------------------------------
// mutate strategy: add qualifier: volatile, const, static
// inputs: used qualifier
//------------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
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
using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
static bool hasMutate = false;
static int vardeclNumber = 0;
static int tmpVarDeclNumber = 0;
static bool hasRandom = false;
static int randomVar = 0;
static bool isFirst = true;
static string mutateType = "";
// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitVarDecl(VarDecl *d) {
    string identifier = mutateType;
    string vartype = d->getType().getAsString();
    if(isFirst == true && vartype.find(identifier) == string::npos){
        vardeclNumber++;
    }
    // cout<<d->getQualifiedNameAsString()<<endl;
    // cout<<d->getType().getAsString()<<endl;
    if(isFirst == false && vartype.find(identifier) == string::npos){
        if(hasRandom == false){
            hasRandom = true;
            cout<<"total: "<<vardeclNumber<<endl;
            randomVar = (rand() % (vardeclNumber-1+1))+ 1;
            cout<<"randdom: "<<randomVar<<endl;
        }
        tmpVarDeclNumber++;
        if(tmpVarDeclNumber == randomVar){
            cout<<"mutate here"<<endl;
            TheRewriter.InsertText(d->getLocStart(),mutateType+" ");
        }
    }
    
    return true;
  }

  bool VisitStmt(Stmt *s) {
    // if (isa<DeclStmt>(s)) {
    //     DeclStmt *ds = cast<DeclStmt>(s);
    //     cout<<ds->getDeclGroup()<<endl;
    // }
    // if (isa<IfStmt>(s)) {
    //     IfStmt *IfStatement = cast<IfStmt>(s);
    //     Stmt *Then = IfStatement->getThen();

    //     TheRewriter.InsertText(Then->getLocStart(), "// the 'if' part\n", true,
    //                          true);

    //     Stmt *Else = IfStatement->getElse();
    //     if (Else)
    //     TheRewriter.InsertText(Else->getLocStart(), "// the 'else' part\n",
    //                            true, true);
    // }
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

  // Override the method that gets called for each parsed top-level
  // declaration.
  /*bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      // Traverse the declaration using our AST visitor.
      Visitor.TraverseDecl(*b);
      isFirst = false;
      cout<<isFirst<<endl;
      Visitor.TraverseDecl(*b);
      //(*b)->dump();
    }
    return true;*/
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
  mutateType = argv[3];
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // ClangTool::run accepts a FrontendActionFactory, which is then used to
  // create new objects implementing the FrontendAction interface. Here we use
  // the helper newFrontendActionFactory to create a default factory that will
  // return a new MyFrontendAction object every time.
  // To further customize this, we could create our own factory class.
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
