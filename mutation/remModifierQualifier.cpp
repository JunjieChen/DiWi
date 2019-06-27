//------------------------------------------------------------------------------
// mutate strategy: remove modifier
// inputs: targeted modifier
//------------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <string.h>
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
static bool isFirst = true;
static string mutateType = "";
// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R) : TheRewriter(R) {}
  LangOptions lopt;
  string decl2str(VarDecl *d) {
    SourceLocation b(d->getLocStart()), _e(d->getLocEnd());
    SourceLocation e(Lexer::getLocForEndOfToken(_e, 0, TheRewriter.getSourceMgr(), lopt));
    return string(TheRewriter.getSourceMgr().getCharacterData(b),
        TheRewriter.getSourceMgr().getCharacterData(e)-TheRewriter.getSourceMgr().getCharacterData(b));
  }

  bool VisitVarDecl(VarDecl *d) {
    string oristr = decl2str(d);
    string identifier = mutateType;
    string inttype = "int";
    string chartype = "char";
    string shorttype = "short";
    string longtype = "long";
    string vartype = d->getType().getAsString();
    if (strcmp(mutateType.c_str(),"unsigned")==0 || strcmp(mutateType.c_str(),"signed")==0 || strcmp(mutateType.c_str(),"short")==0 || strcmp(mutateType.c_str(),"long")==0){
        if (!isa<ParmVarDecl>(d)) {
            // cout<<d->getType().getAsString()<<endl;
            if(strcmp(mutateType.c_str(),"unsigned")==0) {
                if(isFirst == true && oristr.find(identifier) != string::npos){
                    vardeclNumber++;
                }
                if(isFirst == false && oristr.find(identifier) != string::npos){
                    if(hasRandom == false){
                        hasRandom = true;
                        cout<<"total: "<<vardeclNumber<<endl;
                        randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                        cout<<"randdom: "<<randomVar<<endl;
                    }
                    tmpVarDeclNumber++;
                    if(tmpVarDeclNumber == randomVar){
                        cout<<"mutate here"<<endl;

                        string newstr = "";

                        if(oristr.find(inttype) != string::npos
                            || oristr.find(chartype) != string::npos
                            || oristr.find(shorttype) != string::npos
                            || oristr.find(longtype) != string::npos){
                            //remove
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,8+1,"");
                            cout<<"replace: "<<newstr<<endl;
                        } else {
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,8,inttype);
                            cout<<"replace: "<<newstr<<endl;
                        } 
                        TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                    }
                }
            } else if(strcmp(mutateType.c_str(),"signed")==0) {
                string unsignedtype = "unsigned";
                if(isFirst == true 
                    && (oristr.find(identifier) != string::npos 
                        && oristr.find(unsignedtype) == string::npos)){
                    vardeclNumber++;
                }
                if(isFirst == false 
                    && (oristr.find(identifier) != string::npos 
                        && oristr.find(unsignedtype) == string::npos)){
                    if(hasRandom == false){
                        hasRandom = true;
                        cout<<"total: "<<vardeclNumber<<endl;
                        randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                        cout<<"randdom: "<<randomVar<<endl;
                    }
                    tmpVarDeclNumber++;
                    if(tmpVarDeclNumber == randomVar){
                        cout<<"mutate here"<<endl;

                        string newstr = "";

                        if(oristr.find(inttype) != string::npos
                            || oristr.find(chartype) != string::npos
                            || oristr.find(shorttype) != string::npos
                            || oristr.find(longtype) != string::npos){
                            //remove
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,6+1,"");
                            cout<<"replace: "<<newstr<<endl;
                        } else {
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,6,inttype);
                            cout<<"replace: "<<newstr<<endl;
                        } 
                        TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                    }
                }
            } else if(strcmp(mutateType.c_str(),"short")==0) {
                if(isFirst == true && (oristr.find(identifier) != string::npos)){
                    vardeclNumber++;
                }
                if(isFirst == false && (oristr.find(identifier) != string::npos)){
                    if(hasRandom == false){
                        hasRandom = true;
                        cout<<"total: "<<vardeclNumber<<endl;
                        randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                        cout<<"randdom: "<<randomVar<<endl;
                    }
                    tmpVarDeclNumber++;
                    if(tmpVarDeclNumber == randomVar){
                        cout<<"mutate here"<<endl;

                        string newstr = "";

                        if(oristr.find(inttype) != string::npos){
                            //remove
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,5+1,"");
                            cout<<"replace: "<<newstr<<endl;
                        } else {
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,5,inttype);
                            cout<<"replace: "<<newstr<<endl;
                        } 
                        TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                    }
                }
                
            } else if(strcmp(mutateType.c_str(),"long")==0) {
                if(isFirst == true && (oristr.find(identifier) != string::npos)){
                    vardeclNumber++;
                }
                if(isFirst == false && (oristr.find(identifier) != string::npos)){
                    if(hasRandom == false){
                        hasRandom = true;
                        cout<<"total: "<<vardeclNumber<<endl;
                        randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                        cout<<"randdom: "<<randomVar<<endl;
                    }
                    tmpVarDeclNumber++;
                    if(tmpVarDeclNumber == randomVar){
                        cout<<"mutate here"<<endl;

                        string newstr = "";

                        if(oristr.find(inttype) != string::npos){
                            //remove
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,4+1,"");
                            cout<<"replace: "<<newstr<<endl;
                        } else {
                            int pos = oristr.find(identifier);
                            newstr = oristr.replace(pos,4,inttype);
                            cout<<"replace: "<<newstr<<endl;
                        } 
                        TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                    }
                }
            }
        }
    
    } else if (strcmp(mutateType.c_str(),"const")==0 || strcmp(mutateType.c_str(),"volatile")==0 || strcmp(mutateType.c_str(),"static")==0){
        if(strcmp(mutateType.c_str(),"const")==0) {
            if(isFirst == true && (vartype.find(identifier) != string::npos)){
                vardeclNumber++;
            }
            if(isFirst == false && (vartype.find(identifier) != string::npos)){
                if(hasRandom == false){
                    hasRandom = true;
                    cout<<"total: "<<vardeclNumber<<endl;
                    randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                    cout<<"randdom: "<<randomVar<<endl;
                }
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;

                    string newstr = "";

                    //remove
                    int pos = vartype.find(identifier);
                    string orivartypr = vartype;
                    newstr = vartype.replace(pos,5+1,"");
                    cout<<"replace: "<<newstr<<endl;
                    
                    TheRewriter.ReplaceText(d->getLocStart(),orivartypr.size(),newstr);
                }
            }
            
        } else if(strcmp(mutateType.c_str(),"restrict")==0) {
            if(isFirst == true && (vartype.find(identifier) != string::npos)){
                vardeclNumber++;
            }
            if(isFirst == false && (vartype.find(identifier) != string::npos)){
                if(hasRandom == false){
                    hasRandom = true;
                    cout<<"total: "<<vardeclNumber<<endl;
                    randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                    cout<<"randdom: "<<randomVar<<endl;
                }
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;

                    string newstr = "";

                    //remove
                    int pos = vartype.find(identifier);
                    string orivartypr = vartype;
                    newstr = vartype.replace(pos,8+1,"");
                    cout<<"replace: "<<newstr<<endl;
                    
                    TheRewriter.ReplaceText(d->getLocStart(),orivartypr.size(),newstr);
                }
            }
            
        } else if(strcmp(mutateType.c_str(),"volatile")==0) {
            if(isFirst == true && (vartype.find(identifier) != string::npos)){
                vardeclNumber++;
            }
            if(isFirst == false && (vartype.find(identifier) != string::npos)){
                if(hasRandom == false){
                    hasRandom = true;
                    cout<<"total: "<<vardeclNumber<<endl;
                    randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                    cout<<"randdom: "<<randomVar<<endl;
                }
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;

                    string newstr = "";

                    //remove
                    int pos = vartype.find(identifier);
                    string orivartypr = vartype;
                    newstr = vartype.replace(pos,8+1,"");
                    cout<<"replace: "<<newstr<<endl;
                    
                    TheRewriter.ReplaceText(d->getLocStart(),orivartypr.size(),newstr);
                }
            }
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
