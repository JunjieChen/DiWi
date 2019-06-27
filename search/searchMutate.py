import os,random,math,time
import datetime
import os.path
import subprocess as subp
import sys
import thread
from random import choice
sys.path.append('/devdata/Anonymous/CompilerFL/llvmforme/result/script')
import failmessage_llvm_170
# only first-order mutation, and time-contrainted generation

def exccmd(cmd):
    p=os.popen(cmd,"r")
    rs=[]
    line=""
    while True:
         line=p.readline()
         if not line:
              break
         #print line
         #rs.append(line.strip())
    return rs

def secondRoundforQualifier(secondRoundSelected):   
    if secondRoundSelected==1:
        return 'addQualifier;volatile'
    elif secondRoundSelected==2:
        return 'addQualifier;const'
    elif secondRoundSelected==3:
        return 'addQualifier;restrict'
    elif secondRoundSelected==4:
        return 'remModifierQualifier;volatile'
    elif secondRoundSelected==5:
        return 'remModifierQualifier;const'
    else:
        return 'remModifierQualifier;restrict'

def secondRoundforModifier(secondRoundSelected):   
    if secondRoundSelected==1:
        return 'addRepModifier;unsigned'
    elif secondRoundSelected==2:
        return 'addRepModifier;signed'
    elif secondRoundSelected==3:
        return 'addRepModifier;short'
    elif secondRoundSelected==4:
        return 'addRepModifier;long'
    elif secondRoundSelected==5:
        return 'remModifierQualifier;unsigned'
    elif secondRoundSelected==6:
        return 'remModifierQualifier;signed'
    elif secondRoundSelected==7:
        return 'remModifierQualifier;short'
    else:
        return 'remModifierQualifier;long'

def secondRoundforBinaryOp(secondRoundSelected):    
    if secondRoundSelected==1:
        return 'repBinaryOp;+,-'
    elif secondRoundSelected==2:
        return 'repBinaryOp;+,*'
    elif secondRoundSelected==3:
        return 'repBinaryOp;+,/'
    elif secondRoundSelected==4:
        return 'repBinaryOp;+,%'

    elif secondRoundSelected==5:
        return 'repBinaryOp;-,+'
    elif secondRoundSelected==6:
        return 'repBinaryOp;-,*'
    elif secondRoundSelected==7:
        return 'repBinaryOp;-,/'
    elif secondRoundSelected==8:
        return 'repBinaryOp;-,%'

    elif secondRoundSelected==9:
        return 'repBinaryOp;*,+'
    elif secondRoundSelected==10:
        return 'repBinaryOp;*,-'
    elif secondRoundSelected==11:
        return 'repBinaryOp;*,/'
    elif secondRoundSelected==12:
        return 'repBinaryOp;*,%'

    elif secondRoundSelected==13:
        return 'repBinaryOp;/,+'
    elif secondRoundSelected==14:
        return 'repBinaryOp;/,-'
    elif secondRoundSelected==15:
        return 'repBinaryOp;/,*'
    elif secondRoundSelected==16:
        return 'repBinaryOp;/,%'

    elif secondRoundSelected==17:
        return 'repBinaryOp;%,+'
    elif secondRoundSelected==18:
        return 'repBinaryOp;%,-'
    elif secondRoundSelected==19:
        return 'repBinaryOp;%,*'
    elif secondRoundSelected==20:
        return 'repBinaryOp;%,/'

    elif secondRoundSelected==21:
        return 'repBinaryOp;>,>='
    elif secondRoundSelected==22:
        return 'repBinaryOp;>,<'
    elif secondRoundSelected==23:
        return 'repBinaryOp;>,<='
    elif secondRoundSelected==24:
        return 'repBinaryOp;>,=='
    elif secondRoundSelected==25:
        return 'repBinaryOp;>,!='

    elif secondRoundSelected==26:
        return 'repBinaryOp;>=,>'
    elif secondRoundSelected==27:
        return 'repBinaryOp;>=,<'
    elif secondRoundSelected==28:
        return 'repBinaryOp;>=,<='
    elif secondRoundSelected==29:
        return 'repBinaryOp;>=,=='
    elif secondRoundSelected==30:
        return 'repBinaryOp;>=,!='

    elif secondRoundSelected==31:
        return 'repBinaryOp;<,>'
    elif secondRoundSelected==32:
        return 'repBinaryOp;<,>='
    elif secondRoundSelected==33:
        return 'repBinaryOp;<,<='
    elif secondRoundSelected==34:
        return 'repBinaryOp;<,=='
    elif secondRoundSelected==35:
        return 'repBinaryOp;<,!='

    elif secondRoundSelected==36:
        return 'repBinaryOp;<=,>'
    elif secondRoundSelected==37:
        return 'repBinaryOp;<=,>='
    elif secondRoundSelected==38:
        return 'repBinaryOp;<=,<'
    elif secondRoundSelected==39:
        return 'repBinaryOp;<=,=='
    elif secondRoundSelected==40:
        return 'repBinaryOp;<=,!='

    elif secondRoundSelected==41:
        return 'repBinaryOp;==,>'
    elif secondRoundSelected==42:
        return 'repBinaryOp;==,>='
    elif secondRoundSelected==43:
        return 'repBinaryOp;==,<'
    elif secondRoundSelected==44:
        return 'repBinaryOp;==,<='
    elif secondRoundSelected==45:
        return 'repBinaryOp;==,!='

    elif secondRoundSelected==46:
        return 'repBinaryOp;!=,>'
    elif secondRoundSelected==47:
        return 'repBinaryOp;!=,>='
    elif secondRoundSelected==48:
        return 'repBinaryOp;!=,<'
    elif secondRoundSelected==49:
        return 'repBinaryOp;!=,<='
    elif secondRoundSelected==50:
        return 'repBinaryOp;!=,=='

    elif secondRoundSelected==51:
        return 'repBinaryOp;&,^'
    elif secondRoundSelected==52:
        return 'repBinaryOp;&,|'

    elif secondRoundSelected==53:
        return 'repBinaryOp;|,^'
    elif secondRoundSelected==54:
        return 'repBinaryOp;|,&'

    elif secondRoundSelected==55:
        return 'repBinaryOp;^,&'
    elif secondRoundSelected==56:
        return 'repBinaryOp;^,|'

    elif secondRoundSelected==57:
        return 'repBinaryOp;<<,>>'

    elif secondRoundSelected==58:
        return 'repBinaryOp;>>,<<'

    elif secondRoundSelected==59:
        return 'repBinaryOp;&&,||'

    elif secondRoundSelected==60:
        return 'repBinaryOp;||,&&'

    elif secondRoundSelected==61:
        return 'repBinaryOp;+=,-='
    elif secondRoundSelected==62:
        return 'repBinaryOp;+=,*='
    elif secondRoundSelected==63:
        return 'repBinaryOp;+=,/='
    elif secondRoundSelected==64:
        return 'repBinaryOp;+=,%='

    elif secondRoundSelected==65:
        return 'repBinaryOp;-=,+='
    elif secondRoundSelected==66:
        return 'repBinaryOp;-=,*='
    elif secondRoundSelected==67:
        return 'repBinaryOp;-=,/='
    elif secondRoundSelected==68:
        return 'repBinaryOp;-=,%='

    elif secondRoundSelected==69:
        return 'repBinaryOp;*=,+='
    elif secondRoundSelected==70:
        return 'repBinaryOp;*=,-='
    elif secondRoundSelected==71:
        return 'repBinaryOp;*=,/='
    elif secondRoundSelected==72:
        return 'repBinaryOp;*=,%='

    elif secondRoundSelected==73:
        return 'repBinaryOp;/=,+='
    elif secondRoundSelected==74:
        return 'repBinaryOp;/=,-='
    elif secondRoundSelected==75:
        return 'repBinaryOp;/=,*='
    elif secondRoundSelected==76:
        return 'repBinaryOp;/=,%='

    elif secondRoundSelected==77:
        return 'repBinaryOp;%=,+='
    elif secondRoundSelected==78:
        return 'repBinaryOp;%=,-='
    elif secondRoundSelected==79:
        return 'repBinaryOp;%=,*='
    elif secondRoundSelected==80:
        return 'repBinaryOp;%=,/='

    elif secondRoundSelected==81:
        return 'repBinaryOp;<<=,>>='

    elif secondRoundSelected==82:
        return 'repBinaryOp;>>=,<<='

    elif secondRoundSelected==83:
        return 'repBinaryOp;&=,^='
    elif secondRoundSelected==84:
        return 'repBinaryOp;&=,|='

    elif secondRoundSelected==85:
        return 'repBinaryOp;|=,^='
    elif secondRoundSelected==86:
        return 'repBinaryOp;|=,&='

    elif secondRoundSelected==87:
        return 'repBinaryOp;^=,&='
    else:
        return 'repBinaryOp;^=,|='

def secondRoundforUnaryOp(secondRoundSelected):    
    if secondRoundSelected==1:
        return 'repRemUnaryOp;pre++,pre--'
    elif secondRoundSelected==2:
        return 'repRemUnaryOp;pre++,post++'
    elif secondRoundSelected==3:
        return 'repRemUnaryOp;pre++,post--'
    elif secondRoundSelected==4:
        return 'repRemUnaryOp;pre++,~'
    elif secondRoundSelected==5:
        return 'repRemUnaryOp;pre++,delete'

    elif secondRoundSelected==6:
        return 'repRemUnaryOp;pre--,pre++'
    elif secondRoundSelected==7:
        return 'repRemUnaryOp;pre--,post++'
    elif secondRoundSelected==8:
        return 'repRemUnaryOp;pre--,post--'
    elif secondRoundSelected==9:
        return 'repRemUnaryOp;pre--,~'
    elif secondRoundSelected==10:
        return 'repRemUnaryOp;pre--,delete'

    elif secondRoundSelected==11:
        return 'repRemUnaryOp;post++,pre++'
    elif secondRoundSelected==12:
        return 'repRemUnaryOp;post++,pre--'
    elif secondRoundSelected==13:
        return 'repRemUnaryOp;post++,post--'
    elif secondRoundSelected==14:
        return 'repRemUnaryOp;post++,~'
    elif secondRoundSelected==15:
        return 'repRemUnaryOp;post++,delete'

    elif secondRoundSelected==16:
        return 'repRemUnaryOp;post--,pre++'
    elif secondRoundSelected==17:
        return 'repRemUnaryOp;post--,pre--'
    elif secondRoundSelected==18:
        return 'repRemUnaryOp;post--,post++'
    elif secondRoundSelected==19:
        return 'repRemUnaryOp;post--,~'
    elif secondRoundSelected==20:
        return 'repRemUnaryOp;post--,delete'

    elif secondRoundSelected==21:
        return 'repRemUnaryOp;~,pre++'
    elif secondRoundSelected==22:
        return 'repRemUnaryOp;~,pre--'
    elif secondRoundSelected==23:
        return 'repRemUnaryOp;~,post++'
    elif secondRoundSelected==24:
        return 'repRemUnaryOp;~,post--'
    else:
        return 'repRemUnaryOp;~,delete'

def secondRoundforConstant(secondRoundSelected):    
    if secondRoundSelected==1:
        return 'repIntConstant;+1'
    elif secondRoundSelected==2:
        return 'repIntConstant;-1'
    elif secondRoundSelected==3:
        return 'repIntConstant;*-1'
    else:
        return 'repIntConstant;*0'

def secondRoundforVariable():
    
    return 'repVarSameScope;'

'''
def checkIsPass(): # change per bug #to implement
    if os.path.exists('a.out'):
        exccmd('rm a.out')
    exccmd('find '+covdir+' -name \"*.gcda\" | xargs rm -f')
    # exccmd(gccpath+' '+compilationOptionsRight+' mainvar.c')
    exccmd(gccpath+' '+compilationOptionsRight+' mainvar.c')
    if not os.path.exists('a.out'):
        return 0

    if os.path.exists('rightfile'):
        exccmd('rm rightfile')

    start=time.time()
    exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    else:
        if 'core dumped' in lines[0]:
            return 0

    if os.path.exists('a.out'):
        exccmd('rm a.out')
    exccmd('find '+covdir+' -name \"*.gcda\" | xargs rm -f')
    exccmd(gccpath+' '+compilationOptionsWrong+' mainvar.c')
    if not os.path.exists('a.out'):
        return 0

    if os.path.exists('wrongfile'):
        exccmd('rm wrongfile')
    start=time.time()
    exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    else:
        if 'core dumped' in lines[0]:
            return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    lines=f.readlines()
    f.close()
    if len(lines)==0:
        return 1 # pass
    else:
        return 2 # still fail
'''
def diffWithExistingPass(passtestdir):
    if os.path.exists('difffilefail'):
        exccmd('rm difffilefail')
    if os.path.exists(failtestdir+'/fail.c'):
        exccmd('diff mainvar.c '+failtestdir+'/fail.c'+' >difffilefail')
    else:
        exccmd('diff mainvar.c '+failtestdir+'ori'+'/fail.c'+' >difffilefail')
    difffail=open('difffilefail')
    difffaillines=difffail.readlines()
    difffail.close()
    if len(difffaillines)==0:
        return 0 #exist
    for i in range(len(difffaillines)):
        if 'printf' in difffaillines[i]:
            return 0 #faking passing

    for f in os.listdir(passtestdir):
        if os.path.exists('difffile'):
            exccmd('rm difffile')
        exccmd('diff mainvar.c '+passtestdir+'/'+f+' >difffile')
        difff=open('difffile')
        diffflines=difff.readlines()
        difff.close()
        if len(diffflines)==0:
            return 0 #exist
        
    return 1

def collectCov(testname):
    exccmd('mkdir '+resdir+'/passcov/'+testname)
    # exccmd('mkdir '+resdir+'/passcov/'+testname+'/method_info')
    # exccmd('mkdir '+resdir+'/passcov/'+testname+'/stmt_info')
    methodfile=open(resdir+'/passcov/'+testname+'/method_info.txt','w')
    stmtfile=open(resdir+'/passcov/'+testname+'/stmt_info.txt','w')

    if os.path.exists('gcdalist'): # all files to be collected
        exccmd('rm gcdalist')
    exccmd('find '+covdir+' -name \"*.gcda\" > gcdalist')
    # exccmd('find '+srcdir+' -name \"*.h\" >> gcdalist')

    f=open('gcdalist')
    lines=f.readlines()
    f.close()

    for i in range(len(lines)):
        gcdafile=lines[i].strip()
        if '/clang/test/' in gcdafile:
            continue
        # exccmd('cp '+lines[i].strip()+' '+gcdafile)
        exccmd('rm *.gcov')
        if os.path.exists('gcovfile'):
            exccmd('rm gcovfile')
        exccmd(gcovpath+' -f '+gcdafile+' > gcovfile')
        if not os.path.exists('./'+gcdafile.strip().split('/')[-1].split('.gcda')[0]+'.gcov'):
            continue

        f=open('gcovfile')
        gcovlines=f.readlines()
        f.close()

        # result=open(resdir+'/passcov/'+testname+'/method_info/'+gcdafile.strip().split(srcdir+'/')[-1].replace('/','+')+'.txt','w')
        for j in range(len(gcovlines)):
            if 'Function \'' in gcovlines[j].strip():
                if 'Lines executed:' in gcovlines[j+1].strip() and float(gcovlines[j+1].strip().split('Lines executed:')[1].split('%')[0].strip())!=0.0:
                    methodfile.write(gcdafile.split(covdir+'/')[-1]+','+gcovlines[j].strip().split('\'')[1]+','+gcovlines[j+1].strip().split('Lines executed:')[1].split('%')[0].strip()+','+gcovlines[j+1].strip().split('of')[-1].strip()+'\n')
        # result.close()

        f=open(gcdafile.strip().split('/')[-1].split('.gcda')[0]+'.gcov')
        stmtlines=f.readlines()
        f.close()

        # result=open(resdir+'/passcov/'+testname+'/stmt_info/'+gcdafile.strip().split(srcdir+'/')[-1].replace('/','+')+'.txt','w')
        tmp=[]
        for j in range(len(stmtlines)):
            covcnt=stmtlines[j].strip().split(':')[0].strip()
            linenum=stmtlines[j].strip().split(':')[1].strip()
            if covcnt!='-' and covcnt!='#####':
                tmp.append(linenum)
        if len(tmp)==0:
            continue
        stmtfile.write(gcdafile.split(covdir+'/')[-1]+':'+','.join(tmp)+'\n')
        # result.close()
    stmtfile.close()
    methodfile.close()

def selectMutationStrategy():
    mutationStrategy=random.randint(1,132);
    # fistroundSelected=choice(firstRoundStrategy)
    if mutationStrategy>=1 and mutationStrategy<=6:
        secondRoundSelected=mutationStrategy
        commandMutate=secondRoundforQualifier(secondRoundSelected)
    elif mutationStrategy>=7 and mutationStrategy<=14:
        secondRoundSelected=mutationStrategy-6
        commandMutate=secondRoundforModifier(secondRoundSelected)
    elif mutationStrategy>=15 and mutationStrategy<=102:
        secondRoundSelected=mutationStrategy-14
        commandMutate=secondRoundforBinaryOp(secondRoundSelected)
    elif mutationStrategy>=103 and mutationStrategy<=127:
        secondRoundSelected=mutationStrategy-102
        commandMutate=secondRoundforUnaryOp(secondRoundSelected)
    elif mutationStrategy>=128 and mutationStrategy<=131:
        secondRoundSelected=mutationStrategy-127
        commandMutate=secondRoundforConstant(secondRoundSelected)
    else:
        commandMutate=secondRoundforVariable()
    return (commandMutate,mutationStrategy)

def findIndex(tmplist,element):
    for i in range(len(tmplist)):
        if tmplist[i][0]==element:
            return (i,tmplist[i][1])
    return (-1,-1)

def diffWithExistingCov(testname):
    if len(os.listdir(resdir+'/passcov'))==1:
        thisfile=open(resdir+'/passcov/'+testname+'/stmt_info.txt')
        thislines=thisfile.readlines()
        thisfile.close()

        thisset=set()
        for i in range(len(thislines)):
            filenameitem=thislines[i].strip().split(':')[0]
            lineitems=thislines[i].strip().split(':')[1].split(',')
            for j in range(len(lineitems)):
                thisset.add(filenameitem+':'+lineitems[j])
        existingcovset[testname]=thisset&failset
        return 0 # different

    thisfile=open(resdir+'/passcov/'+testname+'/stmt_info.txt')
    thislines=thisfile.readlines()
    thisfile.close()

    thisset=set()
    for i in range(len(thislines)):
        filenameitem=thislines[i].strip().split(':')[0]
        lineitems=thislines[i].strip().split(':')[1].split(',')
        for j in range(len(lineitems)):
            thisset.add(filenameitem+':'+lineitems[j])

    for key in existingcovset.keys():
        similarity=float(len(existingcovset[key]&(thisset&failset)))/len(existingcovset[key]|(thisset&failset))
        # print similarity
        # sys.exit()
        if similarity==1:
            # existingcovset[testname]=thisset&failset
            return 1 # same
    existingcovset[testname]=thisset&failset
    return 0 #different

def diffWithExistingCov_old(testname):
    if len(os.listdir(resdir+'/passcov'))==1:
        return 0 # different

    thisfile=resdir+'/passcov/'+testname+'/stmt_info.txt'
    for f in os.listdir(resdir+'/passcov'):
        if f!=testname:
            passfile=resdir+'/passcov/'+f+'/stmt_info.txt'
            if os.path.exists('covdiff'):
                exccmd('rm covdiff')
            exccmd('diff '+thisfile+' '+passfile+' > covdiff')
            covfile=open('covdiff')
            covlines=covfile.readlines()
            covfile.close()
            if len(covlines)==0:
                return 1 # same
    return 0 #different

def updateScore(tmplist,index2,testname,succval,totalval):
    e1=tmplist[index2][0]
    e2=tmplist[index2][1]

    if len(os.listdir(resdir+'/passcov'))==1:
        tmplist[index2]=(e1,1.0)
        return tmplist

    diversitysum=0.0
    existingnum=0
    thistestset=set()

    for key in existingcovset.keys():
        if key==testname:
            continue
        diversitysum+=(1-(float(len(existingcovset[key]&existingcovset[testname])))/float(len(existingcovset[key]|existingcovset[testname])))
        existingnum+=1

    newe2=(diversitysum/existingnum)*(succval/totalval)
    tmplist[index2]=(e1,newe2)
    return tmplist

def updateScore2(tmplist,index2,testname,succval,totalval):
    e1=tmplist[index2][0]
    e2=tmplist[index2][1]

    if len(os.listdir(resdir+'/passcov'))==1:
        tmplist[index2]=(e1,1.0)
        return tmplist

    diversitysum=0.0
    existingnum=0
    thistestset=set()

    stmtcov=open(resdir+'/passcov/'+testname+'/stmt_info.txt')
    stmtlines=stmtcov.readlines()
    stmtcov.close()

    for i in range(len(stmtlines)):
        filename=stmtlines[i].strip().split(':')[0]
        stmtlist=stmtlines[i].strip().split(':')[1].split(',')
        for j in range(len(stmtlist)):
            thistestset.add(filename+':'+stmtlist[j])

    for f in os.listdir(resdir+'/passcov'):   
        if f!=testname:
            passset=set()
            stmtcov=open(resdir+'/passcov/'+f+'/stmt_info.txt')
            stmtlines=stmtcov.readlines()
            stmtcov.close()

            for i in range(len(stmtlines)):
                filename=stmtlines[i].strip().split(':')[0]
                stmtlist=stmtlines[i].strip().split(':')[1].split(',')

                for j in range(len(stmtlist)):
                    passset.add(filename+':'+stmtlist[j])
            diversitysum+=(1-(float(len(passset&thistestset&failset)))/float(len((passset&failset)|(thistestset&failset))))
            existingnum+=1

    newe2=(diversitysum/existingnum)*(succval/totalval)
    tmplist[index2]=(e1,newe2)
    return tmplist

############## main process    
# change per bug
revisionnumber=sys.argv[1]
compilationOptionsRight=sys.argv[2].replace('+',' ')
compilationOptionsWrong=sys.argv[3].replace('+',' ')
checkpass=sys.argv[4]
roundcount=sys.argv[5]
prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
mcmcprefix='/devdata/Anonymous/CompilerFL/llvmforme/result/MCMC_new_r'+roundcount+'/'+revisionnumber
locationpath='/devdata/Anonymous/CompilerFL/llvmforme/locations/'+revisionnumber[1:]
if os.path.exists(mcmcprefix):
    exccmd('rm -rf '+mcmcprefix)
exccmd('mkdir '+mcmcprefix)
os.chdir(mcmcprefix)
gccpath=prefixpath+'-build/bin/clang' 
gcovpath='gcov'
covdir=prefixpath+'-build'
srcdir=prefixpath
resdir=mcmcprefix
exccmd('cp /devdata/Anonymous/CompilerFL/mutation/* .')
# firstRoundStrategy=['qualifier','modifier','binaryop','unaryop','constant','variable']
if os.path.exists(resdir+'/failtest'):
    exccmd('rm -rf '+resdir+'/failtest')
exccmd('mkdir '+resdir+'/failtest')
exccmd('cp '+locationpath+'/fail.c '+resdir+'/failtest')
exccmd('mkdir '+resdir+'/passcov')
failtestdir='./failtest'
firstfailtestdir='./firstfailtest'
exccmd('mkdir '+firstfailtestdir)
passtestdir='./passtest'
exccmd('mkdir '+passtestdir)
# passorfail=['pass','firstfail','fail','fail','fail','fail','fail','fail','fail','fail']
# passorfail=['firstfail','firstfail','fail','fail','fail','fail','fail','fail','fail','fail']
mutateRecord=open('mutateRecode','w')
successRecord=open('successRecord','w')

exccmd('cp '+failtestdir+'/fail.c '+'./')
if os.path.exists('a.out'):
    exccmd('rm a.out')
exccmd(gccpath+' '+compilationOptionsWrong+' fail.c')
if os.path.exists('oriwrongfile'):
    exccmd('rm oriwrongfile')
if not os.path.exists('a.out'):
    sys.exit(1)
# exccmd('./a.out 2>&1 | tee oriwrongfile')
exccmd('{ timeout 10 ./a.out ; } >oriwrongfile 2>&1')
# print 'start'
failcovpath='/devdata/Anonymous/CompilerFL/llvmforme/locations'
if os.path.exists(failcovpath+'/'+revisionnumber[1:]+'/failcov/stmt_info.txt'):
    tarpath=failcovpath+'/'+revisionnumber[1:]+'/failcov/stmt_info.txt'
elif os.path.exists(failcovpath+'/'+revisionnumber[1:]+'/fail/stmt_info.txt'):
    tarpath=failcovpath+'/'+revisionnumber[1:]+'/fail/stmt_info.txt'
else:
    print "Error!!"
    sys.exit(1)
failfile=open(tarpath)
faillines=failfile.readlines()
failfile.close()
failset=set()
for i in range(len(faillines)):
    filename=faillines[i].strip().split(':')[0]
    stmtlist=faillines[i].strip().split(':')[1].split(',')
    for j in range(len(stmtlist)):
        failset.add(filename+':'+stmtlist[j])

mutationlist=[]
successratelist=dict()
for i in range(1,132+1):
    mutationlist.append((i,0.0)) # mutationNO and metric value; index is ranking
    successratelist[i]=(0.0,0.0) # success and total
(commandMutate,mutationNo)=selectMutationStrategy()

cnt=0
firstfailcnt=0
existingcovset=dict()
failtype='fail'
exccmd('mkdir '+failtestdir+'ori')
starttime=time.time()
while(cnt<500):
    endtime=time.time()
    gaptime=endtime-starttime
    if gaptime>3600:
        if len(os.listdir(passtestdir))>0:
            sys.exit()
        else:
            starttime=time.time()
            exccmd('mv '+failtestdir+'/* '+failtestdir+'ori')
            if len(os.listdir(firstfailtestdir))==0:
                sys.exit()
            exccmd('mv '+firstfailtestdir+'/* '+failtestdir)
            failtype='firstfail'
    ############## select seed program
    mutatefile=''
    # failtype=choice(passorfail)
    if failtype=='fail':
        mutatefile=failtestdir+'/fail.c'
    elif failtype=='firstfail':
        if len(os.listdir(passtestdir))>0:
            sys.exit()
        # if len(os.listdir(failtestdir))==0:
        #     continue
        tmpfile=choice(os.listdir(failtestdir))
        mutatefile=failtestdir+'/'+tmpfile
    # else:
    #     if len(os.listdir(passtestdir))==0:
    #         continue
    #     tmpfile=choice(os.listdir(passtestdir))
    #     mutatefile=passtestdir+'/'+tmpfile

    ############## determine k1
    (k1,v1)=findIndex(mutationlist,mutationNo)
    if k1==-1:
        print 'BUG!!!'
        break
    ############## determie k2
    commandMutate=''
    mutationNo=-1
    k2=-1
    while(1):
        (commandMutate,mutationNo)=selectMutationStrategy()
        (k2,v2)=findIndex(mutationlist,mutationNo)
        if v1==v2:
            diffk2k1=0
        else:
            diffk2k1=k2-k1

        if diffk2k1<=0:
            break
        elif random.random()<(1-0.023)**diffk2k1:# p can be changed
            break
    newtotalvalue=successratelist[mutationNo][1]+1
    successratelist[mutationNo]=(successratelist[mutationNo][0],newtotalvalue)
    
    classfile=commandMutate.strip().split(';')[0]
    inputslist=commandMutate.strip().split(';')[1].split(',')
    for i in range(len(inputslist)):
        inputslist[i]='\"'+inputslist[i]+'\"'
    inputsstr=' '.join(inputslist)
    ############## mutate program
    exccmd('rm *.c')
    exccmd('cp '+mutatefile+' ./main.c')
    mutateRecord.write(mutatefile+';'+commandMutate+';'+str(mutationNo)+'\n')
    mutateRecord.flush()
    if os.path.exists('mainvar.c'):
        exccmd('rm mainvar.c')
    os.system('./'+classfile+' main.c -- '+inputsstr) # mutate to generate new test program
    if os.path.exists('difftmp'):
        exccmd('rm difftmp')
    exccmd('diff main.c mainvar.c > difftmp')
    f=open('difftmp')
    difflines=f.readlines()
    f.close()

    if len(difflines)==0:
        mutationlist[k2]=(mutationlist[k2][0],mutationlist[k2][1]*(successratelist[mutationNo][0]/successratelist[mutationNo][1]))
        mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
        continue
    else:
        if os.path.exists('bugmessage'):
            exccmd('rm bugmessage')
        # exccmd('find '+covdir+' -name \"*.gcda\" | xargs rm -f')
        # exccmd(gccpath+' '+compilationOptions+' mainvar.c 2>&1 | tee bugmessage')
        flagIsPass=-1
        if checkpass=='checkIsPass_wrongcodeOneline':
            flagIsPass=failmessage_llvm_170.checkIsPass_wrongcodeOneline(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail
        elif checkpass=='checkIsPass_zeroandsegmentoneline':
            flagIsPass=failmessage_llvm_170.checkIsPass_zeroandsegmentoneline(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail
        elif checkpass=='checkIsPass_zeroandonenumber':
            flagIsPass=failmessage_llvm_170.checkIsPass_zeroandonenumber(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail
        elif checkpass=='checkIsPass_multilineswrongcode':
            flagIsPass=failmessage_llvm_170.checkIsPass_multilineswrongcode(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail
        elif checkpass=='checkIsPass_onenumberandzero':
            flagIsPass=failmessage_llvm_170.checkIsPass_onenumberandzero(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail
        elif checkpass=='checkIsPass_onenumberandsegmentoneline':
            flagIsPass=failmessage_llvm_170.checkIsPass_onenumberandsegmentoneline(revisionnumber,compilationOptionsRight,compilationOptionsWrong) # 1:pass; 2:still fail

        if flagIsPass==0:
            mutationlist[k2]=(mutationlist[k2][0],mutationlist[k2][1]*(successratelist[mutationNo][0]/successratelist[mutationNo][1]))
            mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
            continue
        elif flagIsPass==2:
            mutationlist[k2]=(mutationlist[k2][0],mutationlist[k2][1]*(successratelist[mutationNo][0]/successratelist[mutationNo][1]))
            mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
            # if failtype!='fail':
            #     continue
            firstfailcnt+=1
            # if len(os.listdir(firstfailtestdir))==5000:
            #     exccmd('rm -rf '+firstfailtestdir+'/'+choice(os.listdir(firstfailtestdir)))
            exccmd('mv mainvar.c '+firstfailtestdir+'/firstfail_'+classfile+str(mutationNo)+'_'+str(firstfailcnt)+'.c')
            continue
        else:
            flagIsExist=diffWithExistingPass(passtestdir)
            if flagIsExist==0:
                mutationlist[k2]=(mutationlist[k2][0],mutationlist[k2][1]*(successratelist[mutationNo][0]/successratelist[mutationNo][1]))
                mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
                continue
            else:
                cnt+=1
                collectCov('pass_'+classfile+str(mutationNo)+'_'+str(cnt))
                flagCovIsRepetitive=diffWithExistingCov('pass_'+classfile+str(mutationNo)+'_'+str(cnt))
                if flagCovIsRepetitive==1:
                    exccmd('rm -rf '+resdir+'/passcov/'+'pass_'+classfile+str(mutationNo)+'_'+str(cnt))
                    mutationlist[k2]=(mutationlist[k2][0],mutationlist[k2][1]*(successratelist[mutationNo][0]/successratelist[mutationNo][1]))
                    mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
                    cnt-=1
                    continue
                else:
                    exccmd('mv mainvar.c '+passtestdir+'/pass_'+classfile+str(mutationNo)+'_'+str(cnt)+'.c')
                    successRecord.write(mutatefile+';'+commandMutate+';'+str(mutationNo)+'\n')
                    successRecord.flush()
                    ############## update for next iteration
                    newtotalvalue=successratelist[mutationNo][0]+1
                    successratelist[mutationNo]=(newtotalvalue,successratelist[mutationNo][1])
                    mutationlist=updateScore(mutationlist,k2,'pass_'+classfile+str(mutationNo)+'_'+str(cnt),successratelist[mutationNo][0],successratelist[mutationNo][1]) #mutationlist[k2]
                    mutationlist=sorted(mutationlist, key=lambda x:x[1], reverse=True)
                    # starttime=time.time()
    print mutationlist
successRecord.close()
mutateRecord.close()

