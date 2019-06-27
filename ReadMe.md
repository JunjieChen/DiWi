Welcome to the homepage of DiWi.


There are five folders:
- benchmark, includes 90 compiler bug informtion (45 GCC bugs and 45 LLVM bugs)

    - gccbugs_summary.txt and llvmbugs_summy.txt: 
		the first column is the bug ID;
		the second column is the buggy revision number;
		the third and fourth column are compilation options detecting the bug; 
		the last column is the used test oracle (referring to failmessage.py in folder search)

	- folders gccbugs and llvmbugs: for each bug:
		fail.c is the failing test program; 
		file "locations" contains the buggy location and fixed revision number, file "version" describes the bug in the bug report; 
		installgcc.py and installllvm.py are the script used to install the corresponding buggy version.


- mutation, includes the implementations of our mutation rules.

	- File "mutationRules.txt" lists all mutation rules used by DiWi, whose totle number is 132.

	- File "addQualifier.cpp" implements the mutation rules that are related to "add qualifier". Input: qualifier name

	- File "addRepModifier.cpp" implements the mutation rules that are related to "add modifier". Input: modifier name

	- File "remModifierQualifier.cpp" implements the mutation rules that are related to "remove modifier" and "remove qualifier". Input: modifier/qualifier name

	- File "repBinaryOp.cpp" implements the mutation rules that are related to "replace binary operator". Inputs: old Operator, new Operator

	- File "RepIntConstant.cpp" implements the mutation rules that are related to "replace constant". Input: used operation for constant.

	- File "repRemUnaryOp.cpp" implements the mutation rules that are related to "replace/remove unary operator". Inputs: old Operator, new Operator ("delete" represents removing it)

	- File "repVarSameScope.cpp" implements the mutation rules that are related to "replace variables under the same scope"


- search, includes the implementation of our MCMC-based search process
	
	- File "searchMutate.py" implements the guided search process to construct a set of passing test programs

	- File "failmessage.py" implements the used test oracle to determine whether a generated test program is passing or failing


- aggregation, includes the implementation of the aggregation based ranking strategy

	- File "aggregationforrank.py" implements the aggregation-based ranking for bug isolation


- exploration, including the implementation of combining the search-based mutated test programs by DiWi and the developer-provided test suite

	- File “Combination.cpp” implements the combination operation between the two kinds of test programs, i.e., inserting a block from the developer-provided test suite to a search-based mutated test program


Welcome more people to enrich the benchmark together for the following research on compiler bug detection, isolation, and fixing! Moreover, we are always actively improving/enriching our dataset and tool!

Thanks very much!

DiWi authors
