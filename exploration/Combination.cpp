#include <sstream>
#include <string>
#include <string.h>
#include <time.h>   
#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <utility>
#include <regex>
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
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


/* Used in a statement, be Context*/
struct MyVariable{
	string type;
	string name;
	bool hasStruct;
	bool hasGlobal;
	MyVariable(string t, string n, bool has){
		type = t;
		name = n;
		hasStruct = has;
		hasGlobal = false;
	}
};

struct MyFunction{
	string types;
	string name;
	MyFunction(string t, string n){
		types = t;
		name = n;
	}
};

struct MyContext{
	Stmt *statement;
	string str_statement;
	vector<MyVariable*> in;
	vector<MyVariable*> defined;
	vector<MyVariable*> used;
	vector<MyFunction*> functions;
	vector<string> inLabels;
	vector<string> definedLabels;
	vector<string> usedLabels;
	bool hasContOrBreak;
	bool hasLoop;
	bool MustInLoop;
	MyContext(){
		hasContOrBreak = false;
		hasLoop = false;
		statement = NULL;
		MustInLoop = false;
	}
};

struct MyCurrContext{
	vector<MyVariable*> defined;
	vector<MyFunction*> functions;
	vector<string> definedLabels;
	bool inLoop;
	MyCurrContext(){
		inLoop = false;
	}
};

struct matchResult{
	Stmt *statement;
	string str_statement;
	vector<pair<string, string>> namepairs;
	matchResult(){
		statement = NULL;
		str_statement = string("");
	}
};

struct BaseFunction{
	FunctionDecl * function;
	string str_function;
	string types;
	string name;
};

struct BaseRecord{
	RecordDecl * record;
	string str_record;
	string name;
};

static vector<struct MyContext *> DataBase;
static vector<struct BaseFunction *> FunctionBase;
static vector<string> headFileBase;
static vector<struct BaseRecord *> RecordBase;

struct MyContext *tempMyContext = new struct MyContext();
struct MyCurrContext *WholeMyCurrContext = new struct MyCurrContext();

static FunctionDecl * firstFunction = NULL;

static struct matchResult MutateMatchResult;
static Stmt * MutatePoint = NULL;
static vector<MyFunction*> MutateAddFunction;
static vector<string> MutateAddStruct;
static int threadshod = 10;
static string curFile;
static bool thefirstRun = true;
static int applyPoints = 0;		//总共有多少可以mutate的地方
static int justPoint = -1;		//最后进行mutate的位置
static bool mutateSuccess = false;
static int currCount = 0; 		//这一轮总共找到了多少个潜在的mutate位置
static string testsuite;

void debug_PrintMyCurrContext(MyCurrContext * curcontext){
	cout << "~~~~~~~~" <<endl;
	vector<MyVariable *> defined = curcontext -> defined;
	int count2 = defined.size();
	for(int j = 0; j < count2; j++)
		cout << defined[j]->type <<" "<< defined[j]->name << endl;

	vector<string> dlabels = curcontext -> definedLabels;
	int count5 = dlabels.size();
	for(int j = 0; j < count5; j++)
		cout << dlabels[j] << endl;
	cout <<"Inloop:";
	cout << curcontext->inLoop << endl;
	cout << "~~~~~~~~" <<endl;
}

void copyCurrContext(MyCurrContext * from, MyCurrContext * to){
	vector<MyVariable *> def = from -> defined;
	int count1 = def.size();
	for(int j = 0; j < count1; j++){
		to -> defined.push_back(new MyVariable(def[j]->type, def[j]->name, false));
	}
	vector<MyFunction *> funcs = from -> functions;
	int count2 = funcs.size();
	for(int j = 0; j < count2; j++){
		to -> functions.push_back(new MyFunction(funcs[j]->types, funcs[j]->name));
	}
	vector<string> defLabels = from -> definedLabels;
	int count3 = defLabels.size();
	for(int j = 0; j < count2; j++){
		to -> definedLabels.push_back(defLabels[j]);
	}
	to -> inLoop = from -> inLoop;
}

bool myFind(string str, string from){
	size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    if(start_pos != 0 && 
    	(  str[start_pos - 1] >= 'a' && str[start_pos - 1] <= 'z'
    	|| str[start_pos - 1] >= 'A' && str[start_pos - 1] <= 'Z'
    	|| str[start_pos - 1] >= '0' && str[start_pos - 1] <= '9'
    	|| str[start_pos - 1] == '_'))
    	return false;
    int end_pos = start_pos + from.length() - 1;
    if(end_pos != str.length() - 1 && 
    	(  str[end_pos + 1] >= 'a' && str[end_pos + 1] <= 'z'
    	|| str[end_pos + 1] >= 'A' && str[end_pos + 1] <= 'Z'
    	|| str[end_pos + 1] >= '0' && str[end_pos + 1] <= '9'
    	|| str[end_pos + 1] == '_'))
    	return false;
    return true;
}


bool check_colon(string str){ //检查第一个字符是不是分号
	char * s = (char *) str.c_str();
	for(int i = 0; i < strlen(s); i++){
		if(s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
			continue;
		if(s[i] == ';')
			return true;
		return false;
	}

}
void calculateDataBase(){
	int BaseSize = DataBase.size();
	int index = 0;

	vector <int> deleteAns;

	cout << "calculate**********" << endl;
	cout << BaseSize << endl;

	while(index < BaseSize){
		//calculate in
		for(int j = 0; j < DataBase[index]->used.size(); j++){
			DataBase[index]->used[0];
			string name = DataBase[index]->used[j]->name;
			bool findName = false;
			for(int k = 0; k < DataBase[index]->defined.size(); k++){
				if (name == DataBase[index]->defined[k]->name){
					findName = true;
					break;  
				}
			}
			if(findName == false){
				DataBase[index] -> in.push_back(DataBase[index]->used[j]);
			}
		}
		//calculate inLabels
		for(int j = 0; j < DataBase[index]->usedLabels.size(); j++){
			string name = DataBase[index]->usedLabels[j];
			bool findName = false;
			for(int k = 0; k < DataBase[index]->definedLabels.size(); k++){
				if (name == DataBase[index]->definedLabels[k]){
					findName = true;
					break;
				}
			}
			if(findName == false){
				DataBase[index] -> inLabels.push_back(DataBase[index]->usedLabels[j]);
			}
		}

		//calculate whether should use struct
		for(int j = 0; j < DataBase[index]->used.size(); j++){
			if(DataBase[index]->used[j]->type.find("struct") != DataBase[index]->used[j]->type.npos){
				DataBase[index]->used[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->in.size(); j++){
			if(DataBase[index]->in[j]->type.find("struct") != DataBase[index]->in[j]->type.npos){
				DataBase[index]->in[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->defined.size(); j++){
			if(DataBase[index]->defined[j]->type.find("struct") != DataBase[index]->defined[j]->type.npos){
				DataBase[index]->defined[j]->hasStruct = true;
			}
		}

		//calculate whether should use union
		for(int j = 0; j < DataBase[index]->used.size(); j++){
			if(DataBase[index]->used[j]->type.find("union") != DataBase[index]->used[j]->type.npos){
				DataBase[index]->used[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->in.size(); j++){
			if(DataBase[index]->in[j]->type.find("union") != DataBase[index]->in[j]->type.npos){
				DataBase[index]->in[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->defined.size(); j++){
			if(DataBase[index]->defined[j]->type.find("union") != DataBase[index]->defined[j]->type.npos){
				DataBase[index]->defined[j]->hasStruct = true;
			}
		}
		
		//calculate whether should use class
		for(int j = 0; j < DataBase[index]->used.size(); j++){
			if(DataBase[index]->used[j]->type.find("class") != DataBase[index]->used[j]->type.npos){
				DataBase[index]->used[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->in.size(); j++){
			if(DataBase[index]->in[j]->type.find("class") != DataBase[index]->in[j]->type.npos){
				DataBase[index]->in[j]->hasStruct = true;
			}
		}
		for(int j = 0; j < DataBase[index]->defined.size(); j++){
			if(DataBase[index]->defined[j]->type.find("class") != DataBase[index]->defined[j]->type.npos){
				DataBase[index]->defined[j]->hasStruct = true;
			}
		}

		if(DataBase[index] -> hasLoop == false && DataBase[index] -> hasContOrBreak == true){
			DataBase[index] -> MustInLoop = true;
		}
		
		
		string str2 = DataBase[index] -> str_statement;
		if(DataBase[index] -> statement -> getStmtClassName() == "DeclStmt"
		|| myFind(str2, "return") == true
		|| myFind(str2, "printf") == true
		|| myFind(str2, "abort") == true
		|| myFind(str2, "exit") == true
		|| check_colon(str2) == true){
			deleteAns.push_back(index);
		}
		
		index ++;
	}

	for(int j = deleteAns.size() - 1; j >= 0; j--){
		DataBase.erase(DataBase.begin() + deleteAns[j]);
	}

}

int Rand(int N) {
	return rand()%N;
}

//辅助函数，生成随机序列
void RandSeq(vector<int> & Nvec, int N) {
	Nvec.clear();
	for (int i = 1; i <= N; ++i) {
		Nvec.push_back(i);
	}
 
	int rn = 0, temp = 0;
	for (int i = N-1; i > 0; --i) {
		rn = Rand(i+1); // 必须是i+1，因为也有可能随机数rn就是i本身,这样就不需要交换
		temp = Nvec[i];
		Nvec[i] = Nvec[rn];
		Nvec[rn] = temp;
	}
	for (int i = N-1; i >= 0; --i) {
		Nvec[i] = Nvec[i] - 1;
	}
}


struct matchResult matchContext(MyCurrContext * here){
	struct matchResult temp;
	int BaseSize = DataBase.size();
	//debug_PrintMyCurrContext(here);
	int randomNum = rand() % (BaseSize);
	int counts = 0;
	vector<int> Nvec;
	RandSeq(Nvec, BaseSize);
	for(int rindex = 0; rindex < BaseSize; rindex ++){
		randomNum = Nvec[rindex];
		MyContext * there = DataBase[randomNum];
		
		// match variable
		bool match_var = true;
		for(int i = 0; i< there->in.size(); i++){
			string name = there->in[i] -> name;
			string type = there->in[i] -> type;
			bool match_this = false;
			vector<int> var_vec;
			RandSeq(var_vec, here->defined.size());

			for(int jj = 0; jj < here->defined.size(); jj++){
 				int j = var_vec[jj];
				if(here->defined[j]->type == type){
					match_this = true;
					temp.namepairs.push_back(make_pair(name, here->defined[j]->name));
					break;
				}
			}
			if(match_this == true)
				continue;
			else{
				match_var = false;
				break;
			}
		}

		//match labels
		bool match_label = true;
		for(int i = 0; i< there->inLabels.size(); i++){
			string name = there->inLabels[i];
			bool match_this = false;
			for(int j = 0; j < here->definedLabels.size(); j++){
				temp.namepairs.push_back(make_pair(name, here->definedLabels[j]));
				break;
			}
			if(match_this == true)
				continue;
			else{
				match_label = false;
				break;
			}
		}

		//match in loop
		bool match_inLoop = true;
		if(there->MustInLoop == true && here->inLoop == false){
			match_inLoop = false;
		}

		if(match_var == false || match_label == false || match_inLoop == false){
			randomNum = rand() % (BaseSize);
			temp.namepairs.clear();
			//continue;
			break; //IMPORTANT 每个testsuite只进行一次！！！
		}

		// check for name redefined.
		for(int i = 0; i<there->defined.size(); i++){
			string name = there->defined[i] -> name;
			bool match_this = false;
			for(int j = 0; j < WholeMyCurrContext->defined.size();j++){
				if(WholeMyCurrContext->defined[j]->name == name){
					match_this = true;
					break;
				}
			}
			if(match_this == true){
				temp.namepairs.push_back(make_pair(name, name+"_mutation_added_one"));
			}
		}

		//check for labels redefined
		for(int i = 0; i<there->definedLabels.size(); i++){
			string name = there->definedLabels[i];
			bool match_this = false;
			for(int j = 0; j < WholeMyCurrContext->definedLabels.size();j++){
				if(WholeMyCurrContext->definedLabels[j] == name) {
					match_this = true;
					break;
				}
			}
			if(match_this == true){
				temp.namepairs.push_back(make_pair(name, name+"_mutation_added_one"));
			}
		}

		MutateAddFunction.clear();
		MutateAddStruct.clear();
		//check for function update
		for(int i = 0; i<there->functions.size(); i++){
			bool match_this = false;
			for(int j = 0; j< WholeMyCurrContext->functions.size(); j++){
				if(WholeMyCurrContext->functions[j]->types == there->functions[i]->types
					&& WholeMyCurrContext->functions[j]->name == there->functions[i]->name){
					match_this = true;
 					break;
				}
			}
			if(match_this == false){
				MyFunction * newfunc = new MyFunction(there->functions[i]->types,there->functions[i]->name);
				MutateAddFunction.push_back(newfunc);
			}
		}

		//check for struct update
		for(int i = 0; i<there->defined.size(); i++){
			if(there->defined[i]->type.find("struct")!= there->defined[i]->type.npos
			|| there->defined[i]->type.find("union")!= there->defined[i]->type.npos
			|| there->defined[i]->type.find("class")!= there->defined[i]->type.npos){
				int len = MutateAddStruct.size();
				bool findsame = false;
				for(int t = 0; t<len; t++){
					string temptype = MutateAddStruct[t];
					if(temptype == there->defined[i]->type){
						findsame = true;
						break;
					}
				}
				if(findsame == false)
					MutateAddStruct.push_back(there->defined[i]->type);
			}
		}
		temp.statement = there->statement;
		temp.str_statement = there -> str_statement;		
		cout << "INSERT STATEMENT" << endl;
		cout << temp.str_statement << endl;
		break;
	}
	return temp;
}


class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
	public:
	MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

	bool VisitFunctionDecl(FunctionDecl *f) {
		string currFunctionName = f -> getNameInfo().getAsString ();
		Stmt * body = f -> getBody();
		string temp = f -> getReturnType().getAsString() + " (";
		int paramnum = f -> getNumParams();
		for(int i = 1; i < paramnum - 1; i++){
			ParmVarDecl * paraDecl = f -> getParamDecl(i);
			temp = temp + paraDecl -> getOriginalType().getAsString() + ", ";
		}
		if(paramnum > 0){
			ParmVarDecl * paraDecl = f -> getParamDecl(paramnum - 1);
			temp = temp + paraDecl -> getOriginalType().getAsString() + ")";
		}
		else{
			temp = temp + ")";
		}
		struct BaseFunction *bf = new BaseFunction();
		bf -> function = f;
		bf -> types = temp;
		bf -> name = f -> getNameInfo().getAsString();
		FunctionBase.push_back(bf);
		return true;
	}

	bool VisitRecordDecl(RecordDecl * fd){
		struct BaseRecord *br = new BaseRecord();
		cout << "^^^^^^^^^^^^^^^^^^HHHHHHHHHHHH^" <<endl;
		cout << fd -> getNameAsString() << endl;
		br->name = string("struct ") + fd -> getNameAsString();	
		br->record = fd;
		RecordBase.push_back(br);
		return true;
	}

	bool VisitCompoundStmt(CompoundStmt *cs) {
		if(cs -> child_begin() == cs -> child_end()){
			return true;
		}
		for (Stmt::child_iterator i = cs -> child_begin(), e = cs -> child_end();i != e;  ++i){
			Stmt *currStmt = *i;
			if (currStmt == NULL){
				continue;
			}
			tempMyContext -> statement = currStmt;
			myvisitChild(currStmt);
			/* Add a new statement */
			DataBase.push_back(tempMyContext);
			struct MyContext *newMyContext = new struct MyContext();
			tempMyContext = newMyContext;
		}
		return true;
	}

	void myvisitChild(Stmt * stmt){
		if (stmt == NULL){
			return;
		}
		if(stmt -> getStmtClassName() == "BreakStmt" || stmt -> getStmtClassName() == "ContinueStmt" || stmt -> getStmtClassName() == "SwitchStmt"){
			tempMyContext -> hasContOrBreak = true;
		}
		if(stmt -> getStmtClassName() == "ForStmt" || stmt -> getStmtClassName() == "WhileStmt"){
			tempMyContext -> hasLoop = true;
		}
		if(stmt -> getStmtClassName() == "GotoStmt"){
			GotoStmt* gostmt = (GotoStmt*) stmt;
			tempMyContext -> usedLabels.push_back(gostmt ->getLabel () -> getStmt() -> getName());
		}
		if(stmt -> getStmtClassName() == "LabelStmt"){
			LabelStmt* labstmt = (LabelStmt*) stmt;
			tempMyContext -> definedLabels.push_back(labstmt -> getName());
		}
		if (stmt -> getStmtClassName() == "DeclRefExpr"){
			DeclRefExpr* curSt = (DeclRefExpr*) stmt;
			ValueDecl * vadec = curSt->getDecl();
			string tempstring = vadec->getType().getAsString();
			if(tempstring.find("(") != tempstring.npos){
				struct MyFunction * newfunc = new MyFunction(vadec->getType().getAsString(), vadec->getNameAsString());
				tempMyContext -> functions.push_back(newfunc);
			}
			else{
				struct MyVariable * newvar = new MyVariable(vadec->getType().getAsString(), vadec->getNameAsString(), false);
				tempMyContext -> used.push_back(newvar);
			}

		}
		if (stmt -> getStmtClassName() == "DeclStmt"){
			DeclStmt* curSt = (DeclStmt*) stmt;
			if( curSt -> decl_begin () == curSt-> decl_end ())
				return;
			for (DeclStmt::decl_iterator i = curSt -> decl_begin(), e = curSt -> decl_end();i != e;  ++i){
				Decl * decl = *i;
				if (decl -> getDeclKindName() == "Var"){
					VarDecl * vadec = (VarDecl*) decl;
					struct MyVariable * newvar = new MyVariable(vadec->getType().getAsString(), vadec->getNameAsString(), false);
					tempMyContext -> defined.push_back(newvar);
				}
			}
		}
		if(stmt -> child_begin() ==  stmt -> child_end()){
			return;
		}
		for (Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
			Stmt *currStmt = *i;
			if (currStmt == NULL){
				continue;
			}
			myvisitChild(currStmt);
		}
	}

	private:
	Rewriter & TheRewriter;

};

class MyASTVisitor2 : public RecursiveASTVisitor<MyASTVisitor2> {
	public:
	MyASTVisitor2(Rewriter &R) : TheRewriter(R) {}

	bool VisitFunctionDecl(FunctionDecl *f) {
		if(firstFunction == NULL)
			firstFunction = f;
		string currFunctionName = f -> getNameInfo().getAsString ();
		Stmt * body = f -> getBody();
		string temp = f -> getReturnType().getAsString() + " (";
		int paramnum = f -> getNumParams();
		if(thefirstRun == true){
			for(int i = 1; i < paramnum - 1; i++){
				ParmVarDecl * paraDecl = f -> getParamDecl(i);
				temp = temp + paraDecl -> getOriginalType().getAsString() + ", ";
				MyVariable * newvar = new MyVariable(paraDecl -> getOriginalType().getAsString(), paraDecl->getNameAsString() , false); 
				WholeMyCurrContext -> defined.push_back(newvar);
			}
			if(paramnum > 0){
				ParmVarDecl * paraDecl = f -> getParamDecl(paramnum - 1);
				temp = temp + paraDecl -> getOriginalType().getAsString() + ")";
				MyVariable * newvar = new MyVariable(paraDecl -> getOriginalType().getAsString(), paraDecl->getNameAsString() , false); 
				WholeMyCurrContext -> defined.push_back(newvar);
			}
			else{
				temp = temp + ")";
			}
			struct MyFunction * func = new MyFunction(temp, f -> getNameInfo().getAsString());
			WholeMyCurrContext ->functions.push_back(func);
			myvisitChild1(body);
		}
		else{
			//拷贝静态变量
			struct MyCurrContext * currContext = new struct MyCurrContext();
			for(int i = 0; i < WholeMyCurrContext -> defined.size(); i++){
				if (WholeMyCurrContext->defined[i]->hasGlobal == true){
					MyVariable * newvar = new MyVariable(WholeMyCurrContext->defined[i]->type, WholeMyCurrContext->defined[i]->name, false);
					newvar -> hasGlobal = true;
					currContext->defined.push_back(newvar);
				}
			}
			//加入参数
			for(int i = 1; i < paramnum - 1; i++){
				ParmVarDecl * paraDecl = f -> getParamDecl(i);
				temp = temp + paraDecl -> getOriginalType().getAsString() + ", ";
				MyVariable * newvar = new MyVariable(paraDecl -> getOriginalType().getAsString(), paraDecl->getNameAsString() , false); 
				currContext -> defined.push_back(newvar);
			}
			if(paramnum > 0){
				ParmVarDecl * paraDecl = f -> getParamDecl(paramnum - 1);
				temp = temp + paraDecl -> getOriginalType().getAsString() + ")";
				MyVariable * newvar = new MyVariable(paraDecl -> getOriginalType().getAsString(), paraDecl->getNameAsString() , false); 
				currContext -> defined.push_back(newvar);
			}
			else{
				temp = temp + ")";
			}
			myvisitChild2(body, currContext);
		}
		return true;
	}

	bool VisitVarDecl(VarDecl * vadec){
		if(thefirstRun == false){
			return true;
		}
		if(vadec ->hasGlobalStorage() == true){
			struct MyVariable * newvar = new MyVariable(vadec->getType().getAsString(), vadec->getNameAsString(), false);
			newvar -> hasGlobal = true;
			WholeMyCurrContext -> defined.push_back(newvar);
		}
		return true;
	}

	void myvisitChild2(Stmt * stmt, MyCurrContext * context){
		if (stmt == NULL){
			return;
		}
		if(stmt -> getStmtClassName() == "LabelStmt"){
			LabelStmt* labstmt = (LabelStmt*) stmt;
			context -> definedLabels.push_back(labstmt -> getName());
		}
		if(stmt -> getStmtClassName() == "DeclStmt"){
			DeclStmt* curSt = (DeclStmt*) stmt;
			if( curSt -> decl_begin () == curSt-> decl_end ())
				return;
			for (DeclStmt::decl_iterator i = curSt -> decl_begin(), e = curSt -> decl_end();i != e;  ++i){
				Decl * decl = *i;
				if (decl -> getDeclKindName() == "Var"){
					VarDecl * vadec = (VarDecl*) decl;
					struct MyVariable * newvar = new MyVariable(vadec->getType().getAsString(), vadec->getNameAsString(), false);
					context -> defined.push_back(newvar);
				}
			}
		}
		if(stmt -> getStmtClassName() == "CompoundStmt"){
			if(stmt -> child_begin() ==  stmt -> child_end()){
				return;
			}
			struct MyCurrContext * newContext = new struct MyCurrContext();
			copyCurrContext(context, newContext);
			for(Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
				Stmt *currStmt = *i;
				if (currStmt == NULL){
					continue;
				}
				if(currStmt -> getStmtClassName() == "ReturnStmt")
					continue;
				myvisitChild2(currStmt, newContext);
				//Mutate
				if(currCount != justPoint){
					currCount++;
					continue;
				}
				currCount++;
				struct matchResult  m = matchContext(newContext);
				if(m.statement == NULL){
					cout << "NULL STATEMENT" << endl;
				}
				else{
					mutateSuccess = true;
					MutatePoint = currStmt;
					MutateMatchResult.statement = m.statement;
					MutateMatchResult.str_statement = m.str_statement;
					int countm = m.namepairs.size();
					cout << "MUTATE HAS FOUND" << endl;
					if(countm != 0)
						cout << "Has to rename variables:" << endl;
					for(int i = 0; i<countm; i++){
						MutateMatchResult.namepairs.push_back(make_pair(m.namepairs[i].first, m.namepairs[i].second));
						cout << m.namepairs[i].first << " " ;
						cout << m.namepairs[i].second << endl ;
					}
				}
			}
			return;
		}
		if(stmt -> getStmtClassName() == "ForStmt" || stmt -> getStmtClassName() == "WhileStmt"){
			if(stmt -> child_begin() ==  stmt -> child_end()){
				return;
			}
			struct MyCurrContext * newContext = new struct MyCurrContext();
			copyCurrContext(context, newContext);
			newContext -> inLoop = true;
			for(Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
				Stmt *currStmt = *i;
				if (currStmt == NULL){
					continue;
				}
				myvisitChild2(currStmt, newContext);
			}
			return;
		}
		if(stmt -> child_begin() ==  stmt -> child_end()){
			return;
		}
		for(Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
			Stmt *currStmt = *i;
			if (currStmt == NULL){
				continue;
			}
			myvisitChild2(currStmt, context);
		}
	}

	void myvisitChild1(Stmt * stmt){
		if (stmt == NULL){
			return;
		}
		if(stmt -> getStmtClassName() == "LabelStmt"){	
			LabelStmt* labstmt = (LabelStmt*) stmt;
			WholeMyCurrContext -> definedLabels.push_back(labstmt -> getName());
		}
		if(stmt -> getStmtClassName() == "DeclStmt"){
			DeclStmt* curSt = (DeclStmt*) stmt;
			if( curSt -> decl_begin () == curSt-> decl_end ())
				return;
			for (DeclStmt::decl_iterator i = curSt -> decl_begin(), e = curSt -> decl_end();i != e;  ++i){
				Decl * decl = *i;
				if (decl -> getDeclKindName() == "Var"){
					VarDecl * vadec = (VarDecl*) decl;
					struct MyVariable * newvar = new MyVariable(vadec->getType().getAsString(), vadec->getNameAsString(), false);
					WholeMyCurrContext -> defined.push_back(newvar); //全局定义的所有变量
				}
			}
		}
		if(stmt -> getStmtClassName() == "CompoundStmt"){
			if(stmt -> child_begin() ==  stmt -> child_end()){
				return;
			}
			for(Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
				Stmt *currStmt = *i;
				if (currStmt == NULL){
					continue;
				}
				if(currStmt->getStmtClassName() == "ReturnStmt")
					continue;

				myvisitChild1(currStmt);
				applyPoints++;
			}
			return;
		}
		if(stmt -> child_begin() ==  stmt -> child_end()){
			return;
		}
		for(Stmt::child_iterator i = stmt -> child_begin(), e = stmt -> child_end();i != e;  ++i){
			Stmt *currStmt = *i;
			if (currStmt == NULL){
				continue;
			}
			myvisitChild1(currStmt);
		}
	}

	private:
	Rewriter & TheRewriter;

};

class MyASTConsumer : public ASTConsumer {
	public:
	MyASTConsumer(Rewriter &R) : Visitor(R) {}

	void HandleTranslationUnit(ASTContext &Context) {
		Visitor.TraverseDecl(Context.getTranslationUnitDecl());
	}
	private:
	MyASTVisitor Visitor;
};

class MyASTConsumer2 : public ASTConsumer {
	public:
	MyASTConsumer2(Rewriter &R) : Visitor(R) {}

	void HandleTranslationUnit(ASTContext &Context) {
		if(curFile.compare("main.c") != 0){
			return;
		}
		Visitor.TraverseDecl(Context.getTranslationUnitDecl());
		thefirstRun = false;

		vector<int> mutatePlace;
		RandSeq(mutatePlace, applyPoints);
		for(int i = 0; i < applyPoints; i++){
			currCount = 0;
			justPoint = mutatePlace[i];
			Visitor.TraverseDecl(Context.getTranslationUnitDecl());
			if(mutateSuccess == true)
				break;
		}
	}
	private:
	MyASTVisitor2 Visitor;
};

class MyFrontendAction : public ASTFrontendAction {
	public:
	MyFrontendAction() {}

	void EndSourceFileAction() override {
		SourceManager &SM = TheRewriter.getSourceMgr();
		llvm::errs() << "** EndSourceFileAction for: "
					 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
		
		int BaseSize = DataBase.size();
		for(int index = 0; index < BaseSize; index++){

			SourceRange range = DataBase[index] -> statement -> getSourceRange();
			while(1){
				 SourceLocation temp = clang::Lexer::getLocForEndOfToken(range.getEnd(), 0, SM, LangOptions());
				 if(temp.printToString(SM) == range.getEnd().printToString(SM))
				 	break;
				range.setEnd(temp);			
			}
			llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, LangOptions());

			DataBase[index] -> str_statement = ref.str();
		}
		int funSize = FunctionBase.size();
		for(int index = 0; index < funSize; index++){
			SourceRange range = FunctionBase[index] -> function -> getSourceRange();
			while(1){
				 SourceLocation temp = clang::Lexer::getLocForEndOfToken(range.getEnd(), 0, SM, LangOptions());
				 if(temp.printToString(SM) == range.getEnd().printToString(SM))
				 	break;
				range.setEnd(temp);			
			}
			llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, LangOptions());
			FunctionBase[index] -> str_function = ref.str();
		}
		int recSize = RecordBase.size();
		for(int index = 0; index < recSize; index++){
			SourceRange range = RecordBase[index] -> record -> getSourceRange();
			while(1){
				 SourceLocation temp = clang::Lexer::getLocForEndOfToken(range.getEnd(), 0, SM, LangOptions());
				 if(temp.printToString(SM) == range.getEnd().printToString(SM))
				 	break;
				range.setEnd(temp);			
			}
			llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, LangOptions());
			RecordBase[index] -> str_record = ref.str();
		}

		calculateDataBase();
	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
		llvm::errs() << "** Creating AST consumer for: " << file << "\n";
		TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
		return llvm::make_unique<MyASTConsumer>(TheRewriter);
	}

	private:
	Rewriter TheRewriter;
};

class MyFrontendAction2 : public ASTFrontendAction {
	public:
	MyFrontendAction2() {}

	int CopyFile(char *SourceFile,char *NewFile) {
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

	int AddHeader(char *SourceFile, char * TestSuite) {
		ifstream in;
		ofstream out;
		in.open(TestSuite);
		if(in.fail()){
			cout<<"Error 1: Fail to open the source file."<<endl;
			in.close();
			out.close();
			return 0;
		}
		else{
			string line;
			while(getline(in, line)){
				char  * line2 = (char *)line.c_str();
				int len = strlen(line2);
				int first = 0;
				for(int i = 0;i < len; i++){
					if (line2[i]!=' ' || line2[i]!='\t' || line2[i] != '\n')
					{
						first = i;
						break;
					}
				}
				string line3 = line.substr(first);
				if(line3.find("#include") == 0){
					headFileBase.push_back(line);
				}
			}
		}

		in.close();
		vector<string> base;

		in.open(SourceFile);
		if(in.fail())//
		{
			cout<<"Error 1: Fail to open the source file."<<endl;
			in.close();
			out.close();
			return 0;
		}
		else{
			string line;
			while(getline(in, line)){
				base.push_back(line);
			}
		}
		in.close();

		out.open(SourceFile,ios::binary);
		if(out.fail())
		{
			cout<<"Error 2: Fail to create the new file."<<endl;
			out.close();
			in.close();
			return 0;
		}
		else//
		{
			for(int i = 0;i < headFileBase.size(); i++)
				out << headFileBase[i] << endl;
			for(int i = 0;i < base.size(); i++)
				out << base[i]<<endl;
			out.close();
			return 1;
		}
	}

	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while(std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2-pos1));
			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if(pos1 != s.length())
			v.push_back(s.substr(pos1));
	}

	bool replace(std::string& str, const std::string& from, const std::string& to) {
	    while(1){
		    size_t start_pos = str.find(from);
		    if(start_pos == std::string::npos)
		        return false;
		    if(start_pos != 0 && 
		    	(  str[start_pos - 1] >= 'a' && str[start_pos - 1] <= 'z'
		    	|| str[start_pos - 1] >= 'A' && str[start_pos - 1] <= 'Z'
		    	|| str[start_pos - 1] >= '0' && str[start_pos - 1] <= '9'
		    	|| str[start_pos - 1] == '_'))
		    	return false;
		    int end_pos = start_pos + from.length() - 1;
		    if(end_pos != str.length() - 1 && 
		    	(  str[end_pos + 1] >= 'a' && str[end_pos + 1] <= 'z'
		    	|| str[end_pos + 1] >= 'A' && str[end_pos + 1] <= 'Z'
		    	|| str[end_pos + 1] >= '0' && str[end_pos + 1] <= '9'
		    	|| str[end_pos + 1] == '_'))
		    	return false;
		    str.replace(start_pos, from.length(), to);
		}
	}

	void EndSourceFileAction() override {
		SourceManager &SM = TheRewriter.getSourceMgr();
		llvm::errs() << "** EndSourceFileAction for: "
					 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
		if(curFile.compare("main.c") != 0){
			return;
		}
		// apply mutation
		if(MutateMatchResult.statement == NULL || MutateMatchResult.str_statement == string(""))
			return; 
		string insert_text = string("\n") + MutateMatchResult.str_statement;
		int paircount = MutateMatchResult.namepairs.size();
		for(int i = 0; i < paircount; i++){
			string first = MutateMatchResult.namepairs[i].first;
			string second = MutateMatchResult.namepairs[i].second;
			cout << "EndSourceFileAction" << first << " " << second << endl;
			replace(insert_text, first, second);
		}

		if(MutatePoint == NULL)
			return;
		SourceRange range = MutatePoint -> getSourceRange();
		SourceLocation temp;
		while(1){
			temp = clang::Lexer::getLocForEndOfToken(range.getEnd(), 0, SM, LangOptions());
			if(temp.printToString(SM) == range.getEnd().printToString(SM))
				break;
			range.setEnd(temp);			
		}
		TheRewriter.InsertText(temp, insert_text);


		//插入需要的struct
		vector<int> data2;
		int addstructsize = MutateAddStruct.size();
		for(int i = 0; i < addstructsize; i++){
			string stru = MutateAddStruct[i];
			vector<string> temp1;
			SplitString(stru, temp1, " ");
			string name1 = temp1[temp1.size() - 1];
			cout << name1 <<"^^^^^^^^^^^" << endl;
			for(int j = 0; j< RecordBase.size(); j++){
				vector<string> temp2;
				SplitString(RecordBase[j]->name, temp2, " ");
				string name2 = temp2[temp2.size() - 1];
				cout << name2 <<"^^^^^^^^^^^" << endl;
				if (name1 == name2){
					if(count(data2.begin(), data2.end(), j))
						continue;
					data2.push_back(j);
					insert_text = RecordBase[j]->str_record + "\n";
					TheRewriter.InsertText(firstFunction->getLocStart(), insert_text);
				}
			}
		}

		//插入需要的函数
		vector<int> data;
		int addfuncsize = MutateAddFunction.size();

		cout << MutateAddFunction.size() << endl;
		for(int i = 0; i < addfuncsize; i++){
			MyFunction * myfunc = MutateAddFunction[i];
			cout <<  myfunc -> types << " " ;
			cout << myfunc -> name << endl;
			for(int j = 0; j< FunctionBase.size(); j++){
				cout <<  FunctionBase[j] -> types << " " ;
				cout << FunctionBase[j] -> name << endl;
				if (FunctionBase[j]->types == myfunc->types
					&& FunctionBase[j]->name == myfunc->name){
					if(count(data.begin(), data.end(), j))
						continue;
					data.push_back(j);
					insert_text = FunctionBase[j]->str_function + "\n";
					TheRewriter.InsertText(firstFunction->getLocStart(), insert_text);
				}
			}
		}

		if(mutateSuccess){
			// Now emit the rewritten buffer.
			CopyFile("main.c", "mainori.c");
			TheRewriter.overwriteChangedFiles();
			rename("main.c", "mainvar.c");
			rename("mainori.c", "main.c");
			if (addfuncsize != 0)
				AddHeader("mainvar.c", (char*)testsuite.c_str());
		}


	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
													 StringRef file) override {
		vector<string> v;
		SplitString(file.str(), v, "/");
		curFile = v[v.size() - 1];
		llvm::errs() << "** Creating AST consumer for: " << file << "\n";
		TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
		return llvm::make_unique<MyASTConsumer2>(TheRewriter);
	}

	private:
	Rewriter TheRewriter;
};


void debug_PrintDataBase(){
	int count = DataBase.size();
	cout << "PRINT DataBase| Total " << count << "Seeds." << endl;
	for (int i = 0; i < count;i++)
	{
		cout << DataBase[i]->str_statement<<endl;
		vector<MyVariable *> in = DataBase[i] -> in;
		cout << "in: " << endl;
		int count1 = in.size();
		for(int j = 0; j < count1; j++)
			cout << in[j]->type <<" "<< in[j]->name << endl;

		cout << "defined: " << endl;
		vector<MyVariable *> defined = DataBase[i] -> defined;
		int count2 = defined.size();
		for(int j = 0; j < count2; j++)
			cout << defined[j]->type <<" "<< defined[j]->name << endl;

		cout << "used: " << endl;
		vector<MyVariable *> used = DataBase[i] -> used;
		count2 = used.size();
		for(int j = 0; j < count2; j++)
			cout << used[j]->type <<" "<< used[j]->name << endl;

		cout << "functions" << endl;
		vector<MyFunction *> funcs = DataBase[i] -> functions;
		int count3 = funcs.size();
		cout << count3 << endl;
		for(int j = 0; j < count3; j++)
			cout << funcs[j]->types <<" "<< funcs[j]->name << endl;

		cout << "inLabels: " << endl;
		vector<string> inLabels = DataBase[i] -> inLabels;
		int count4 = inLabels.size();
		for(int j = 0; j < count4; j++)
			cout << inLabels[j] << endl;

		cout << "definedLabels: " << endl;
		vector<string> dlabels = DataBase[i] -> definedLabels;
		int count5 = dlabels.size();
		for(int j = 0; j < count5; j++)
			cout << dlabels[j] << endl;

	}
}


int main(int argc, const char ** argv){

	srand((unsigned) time (NULL));
	//将命令行参数分成两部分，分别对应test suite 和 fail test
	int argc_2 = 3;
	int argc_1 = argc - 1;

	char *s = (char *)argv[2];
	argv[2] = argv[3];
	argv[3] = s;

	char ** argv_1;
	argv_1 = (char **)malloc(sizeof(char*) * (argc_1));
	int temp_len = strlen(argv[0]);
	argv_1[0] = (char *)malloc(sizeof(char) * (temp_len+2));
	strcpy(argv_1[0], argv[0]);
	for(int i = 1; i< argc_1; i++){
		int temp_len = strlen(argv[1 + i]);
		argv_1[i] = (char *)malloc(sizeof(char) * (temp_len+2));
		strcpy(argv_1[i], argv[1 + i]);
	}
	const char ** argv1 = (const char ** )argv_1;
	
	llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
	string name0 = string(argv1[1]).substr(0, strlen(argv1[1]) - 2);
	string name1 = name0 + "_var.c";
	argv1[1] = name1.c_str();
	testsuite = argv1[1];
	CommonOptionsParser op1(argc_1, argv1, ToolingSampleCategory);
	ClangTool Tool1(op1.getCompilations(), op1.getSourcePathList());
	Tool1.run(newFrontendActionFactory<MyFrontendAction>().get());

	cout << "---------" << endl;

	CommonOptionsParser op2(argc, argv, ToolingSampleCategory);
	ClangTool Tool2(op2.getCompilations(), op2.getSourcePathList());
	Tool2.run(newFrontendActionFactory<MyFrontendAction2>().get());

	//debug_PrintDataBase()
	return 0;
}
