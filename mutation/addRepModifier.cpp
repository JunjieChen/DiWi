//------------------------------------------------------------------------------
// mutate strategy: add or replace modifier
// inputs: targeted modifier unsigned, signed, short, long
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
    if (!isa<ParmVarDecl>(d)) {
        //do not consider parameter decl
        // cout<<d->getType().getAsString()<<endl;
        if(strcmp(mutateType.c_str(),"unsigned")==0) {
            string identifier = mutateType;
            string inttype = "int";
            string chartype = "char";
            string shorttype = "short";
            string longtype = "long";
            string doubletype = "double";
            string oristr = decl2str(d);

            if(isFirst == true && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(chartype) != string::npos 
                    || oristr.find(shorttype) != string::npos
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
                vardeclNumber++;
            }
            // cout<<d->getQualifiedNameAsString()<<endl;
            // cout<<d->getType().getAsString()<<endl;
            if(isFirst == false && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(chartype) != string::npos 
                    || oristr.find(shorttype) != string::npos
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
                if(hasRandom == false){
                    hasRandom = true;
                    cout<<"total: "<<vardeclNumber<<endl;
                    randomVar = (rand() % (vardeclNumber-1+1))+ 1;
                    cout<<"randdom: "<<randomVar<<endl;
                }
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;

                    string signedtype = "signed";
                    
                    string newstr = "";

                    if(oristr.find(signedtype) != string::npos){
                        //replace
                        int pos = oristr.find(signedtype);
                        newstr = oristr.replace(pos,6,mutateType);
                        cout<<"replace: "<<newstr<<endl;
                    } else if(oristr.find(shorttype) != string::npos){
                        int pos = oristr.find(shorttype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    } else if(oristr.find(longtype) != string::npos){
                        int pos = oristr.find(longtype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    } else{
                        //insert
                        if(oristr.find(inttype) != string::npos){
                            int pos = oristr.find(inttype);
                            newstr = oristr.insert(pos,mutateType+" ");
                            cout<<newstr<<endl;
                        } else{
                            int pos = oristr.find(chartype);
                            newstr = oristr.insert(pos,mutateType+" ");
                            cout<<newstr<<endl;
                        }
                    }
                    TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                }
            }
        } else if(strcmp(mutateType.c_str(),"signed")==0) {
            string identifier = mutateType;
            string inttype = "int";
            string chartype = "char";
            string shorttype = "short";
            string longtype = "long";
            string doubletype = "double";
            string signedtype = "unsigned";
            string oristr = decl2str(d);

            if(isFirst == true 
                && (oristr.find(signedtype) != string::npos || oristr.find(identifier) == string::npos) 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(chartype) != string::npos 
                    || oristr.find(shorttype) != string::npos
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
                vardeclNumber++;
            }
            // cout<<d->getQualifiedNameAsString()<<endl;
            // cout<<d->getType().getAsString()<<endl;
            if(isFirst == false 
                && (oristr.find(signedtype) != string::npos || oristr.find(identifier) == string::npos) 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(chartype) != string::npos 
                    || oristr.find(shorttype) != string::npos
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
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
                    
                    if(oristr.find(signedtype) != string::npos){
                        //replace
                        int pos = oristr.find(signedtype);
                        newstr = oristr.replace(pos,8,mutateType);
                        cout<<"replace: "<<newstr<<endl;
                    } else if(oristr.find(shorttype) != string::npos){
                        int pos = oristr.find(shorttype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    } else if(oristr.find(longtype) != string::npos){
                        int pos = oristr.find(longtype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    } else{
                        //insert
                        if(oristr.find(inttype) != string::npos){
                            int pos = oristr.find(inttype);
                            newstr = oristr.insert(pos,mutateType+" ");
                            cout<<newstr<<endl;
                        } else{
                            int pos = oristr.find(chartype);
                            newstr = oristr.insert(pos,mutateType+" ");
                            cout<<newstr<<endl;
                        }
                    }
                    TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                }
            }

        } else if(strcmp(mutateType.c_str(),"short")==0) {
            string identifier = mutateType;
            string inttype = "int";
            string longtype = "long";
            string doubletype = "double";
            string oristr = decl2str(d);
            if(isFirst == true && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
                vardeclNumber++;
            }
            // cout<<d->getQualifiedNameAsString()<<endl;
            // cout<<d->getType().getAsString()<<endl;
            if(isFirst == false && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || (oristr.find(longtype) != string::npos && oristr.find(doubletype) == string::npos))){
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

                    if(oristr.find(longtype) != string::npos){
                        int pos = oristr.find(longtype);
                        newstr = oristr.replace(pos,4,mutateType);
                        cout<<"replace: "<<newstr<<endl;
                    } else{
                        //insert
                        int pos = oristr.find(inttype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    }
                    TheRewriter.ReplaceText(d->getSourceRange(),newstr);
                }
            }

        } else if(strcmp(mutateType.c_str(),"long")==0) {
            string identifier = mutateType;
            string inttype = "int";
            string shorttype = "short";
            string oristr = decl2str(d);
            if(isFirst == true && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(shorttype) != string::npos)){
                vardeclNumber++;
            }
            // cout<<d->getQualifiedNameAsString()<<endl;
            // cout<<d->getType().getAsString()<<endl;
            if(isFirst == false && oristr.find(identifier) == string::npos 
                && (oristr.find(inttype) != string::npos 
                    || oristr.find(shorttype) != string::npos)){
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

                    if(oristr.find(shorttype) != string::npos){
                        int pos = oristr.find(shorttype);
                        newstr = oristr.replace(pos,5,mutateType);
                        cout<<"replace: "<<newstr<<endl;
                    } else{
                        //insert
                        int pos = oristr.find(inttype);
                        newstr = oristr.insert(pos,mutateType+" ");
                        cout<<newstr<<endl;
                    }
                    TheRewriter.ReplaceText(d->getSourceRange(),newstr);
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
