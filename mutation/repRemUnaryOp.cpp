//------------------------------------------------------------------------------
// mutation strategy: replace/remove unary operation
// inputs: oldOp, newOp
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
// static string mutateType = "";
static string oldOp = "";
static string newOp = "";
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

  bool VisitUnaryOperator(UnaryOperator *u) {
    if(strcmp(oldOp.c_str(),"pre++")==0) { //
        if(u->getOpcode() == UO_PreInc) { //
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
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;
                    if(strcmp(newOp.c_str(),"pre--")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "--"); 
                    } else if (strcmp(newOp.c_str(),"post++")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "++"); 
                    } else if (strcmp(newOp.c_str(),"post--")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "--"); 
                    } else if (strcmp(newOp.c_str(),"~")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "~"); 
                    } else if (strcmp(newOp.c_str(),"delete")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                    }
                }
            }
        }  
    } else if(strcmp(oldOp.c_str(),"pre--")==0) { //
        if(u->getOpcode() == UO_PreDec) { //
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
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;
                    if(strcmp(newOp.c_str(),"pre++")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "++"); 
                    } else if (strcmp(newOp.c_str(),"post++")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "++"); 
                    } else if (strcmp(newOp.c_str(),"post--")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "--"); 
                    } else if (strcmp(newOp.c_str(),"~")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "~"); 
                    } else if (strcmp(newOp.c_str(),"delete")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                    }
                }
            }
        }  
    } else if(strcmp(oldOp.c_str(),"post++")==0) { //
        if(u->getOpcode() == UO_PostInc) { //
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
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;
                    if(strcmp(newOp.c_str(),"post--")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "--"); 
                    } else if (strcmp(newOp.c_str(),"pre++")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "++"); 
                    } else if (strcmp(newOp.c_str(),"pre--")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "--"); 
                    } else if (strcmp(newOp.c_str(),"~")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "~"); 
                    } else if (strcmp(newOp.c_str(),"delete")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                    }
                }
            }
        }  
    } else if(strcmp(oldOp.c_str(),"post--")==0) { //
        if(u->getOpcode() == UO_PostDec) { //
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
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;
                    if(strcmp(newOp.c_str(),"post++")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "++"); 
                    } else if (strcmp(newOp.c_str(),"pre++")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "++"); 
                    } else if (strcmp(newOp.c_str(),"pre--")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "--"); 
                    } else if (strcmp(newOp.c_str(),"~")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextBefore(u->getLocStart(), "~"); 
                    } else if (strcmp(newOp.c_str(),"delete")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                    }
                }
            }
        }  
    } else if(strcmp(oldOp.c_str(),"~")==0) { //
        if(u->getOpcode() == UO_Not) { //
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
                tmpVarDeclNumber++;
                if(tmpVarDeclNumber == randomVar){
                    cout<<"mutate here"<<endl;
                    if(strcmp(newOp.c_str(),"pre++")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "++"); 
                    } else if(strcmp(newOp.c_str(),"pre--")==0){
                        TheRewriter.ReplaceText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), "--"); 
                    } else if (strcmp(newOp.c_str(),"post++")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "++"); 
                    } else if (strcmp(newOp.c_str(),"post--")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                        TheRewriter.InsertTextAfterToken(u->getLocEnd(), "--"); 
                    } else if (strcmp(newOp.c_str(),"delete")==0){
                        Rewriter::RewriteOptions opts;
                        TheRewriter.RemoveText(u->getOperatorLoc(), u->getOpcodeStr(u->getOpcode()).size(), opts); 
                    }
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
  oldOp = argv[3];
  newOp = argv[4];
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // ClangTool::run accepts a FrontendActionFactory, which is then used to
  // create new objects implementing the FrontendAction interface. Here we use
  // the helper newFrontendActionFactory to create a default factory that will
  // return a new MyFrontendAction object every time.
  // To further customize this, we could create our own factory class.
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
