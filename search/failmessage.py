import os,random,math,time
import datetime
import os.path
import subprocess as subp
import sys
import thread
from random import choice

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

'''
$ gcc-trunk -O0 small.c
$ ./a.out 
0
$ gcc-4.6 -O1 small.c
$ ./a.out 
0
$ gcc-trunk -O1 small.c
$ ./a.out 
1
$
'''
def checkIsPass_wrongcodeOneline(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

    if os.path.exists('a.out'):
        exccmd('rm a.out')
    exccmd('find '+covdir+' -name \"*.gcda\" | xargs rm -f')
    # exccmd(gccpath+' '+compilationOptionsRight+' mainvar.c')
    exccmd(gccpath+' '+compilationOptionsRight+' mainvar.c')
    if not os.path.exists('a.out'):
        return 0 # compilation error

    if os.path.exists('rightfile'):
        exccmd('rm rightfile')

    start=time.time()
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    else:
        if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    else:
        if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
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
$ gcc-trunk -O2 small.c; a.out
$ gcc-4.8 -O3 small.c; a.out
$ gcc-trunk -O3 small.c; a.out
Segmentation fault (core dumped)
$ 
'''
def checkIsPass_zeroandsegmentoneline(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

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
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=0:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    # if len(lines)!=1:
    #     return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    diffmesslines=f.readlines()
    f.close()

    if os.path.exists('diffow'):
        exccmd('rm diffow')
    exccmd('diff wrongfile oriwrongfile > diffow')
    f=open('diffow')
    diffowlines=f.readlines()
    f.close()

    if len(diffmesslines)==0:
        return 1 # pass
    else:
        if len(diffowlines)==0: # 'core dumped' in lines[0]:
            return 2 # still fail
        else:
            return 0

def checkIsPass_zeroandonenumber(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

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
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=0:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    # if len(lines)!=1:
    #     return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    diffmesslines=f.readlines()
    f.close()
    if len(diffmesslines)==0:
        return 1 # pass
    else:
        if len(lines)==1 and 'core dumped' not in lines[0] and 'dumped core' not in lines[0] and 'exception' not in lines[0] and 'Abort' not in lines[0] and 'Segmentation' not in lines[0]:
            return 2 # still fail
        else:
            return 0

'''
$ clang-trunk -m32 -O0 small.c; ./a.out
0
4
0
4
0
4
$ clang-trunk -m32 -O1 small.c; ./a.out
0
0
0
$
'''
def checkIsPass_multilineswrongcode(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

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
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)<=1:
        return 0
    if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    if len(lines)<=1:
        return 0
    if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
        return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    diffmesslines=f.readlines()
    f.close()
    if len(diffmesslines)==0:
        return 1 # pass
    else:
        return 2 # still fail

'''
$ clang -m32 -O1 small.c; ./a.out
0 
$ clang -m32 -Os small.c; ./a.out
'''
def checkIsPass_onenumberandzero(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

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
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    # if len(lines)!=1:
    #     return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    diffmesslines=f.readlines()
    f.close()
    if len(diffmesslines)==0:
        return 1 # pass
    else:
        if len(lines)==0:
            return 2 # still fail
        else:
            return 0
'''
$ clang-trunk -O2 small.c
$ a.out
Floating point exception (core dumped)
$ clang-trunk -O3 small.c
$ a.out
Floating point exception (core dumped)
$ clang-3.3 -O3 small.c
$ a.out
Floating point exception (core dumped)
$ clang-3.2 -O3 small.c
$ a.out
0
$ 
'''
def checkIsPass_onenumberandsegmentoneline(RevisionNumber,OptionsRight,OptionsWrong): # change per bug
    revisionnumber=RevisionNumber
    compilationOptionsRight=OptionsRight
    compilationOptionsWrong=OptionsWrong

    prefixpath='/devdata/Anonymous/CompilerFL/llvmforme/'+revisionnumber[:7]+'/'+revisionnumber[:7]
    gccpath=prefixpath+'-build/bin/clang' 
    covdir=prefixpath+'-build'

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
    # exccmd('timeout 10 ./a.out 2>&1 | tee rightfile')
    exccmd('{ timeout 10 ./a.out ; } >rightfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('rightfile')
    lines=f.readlines()
    f.close()
    if len(lines)!=1:
        return 0
    if 'core dumped' in lines[0] or 'dumped core' in lines[0] or 'exception' in lines[0] or 'Abort' in lines[0] or 'Segmentation' in lines[0]:
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
    # exccmd('timeout 10 ./a.out 2>&1 | tee wrongfile')
    exccmd('{ timeout 10 ./a.out ; } >wrongfile 2>&1')
    end=time.time()
    if (end-start)>=10:
        return 0

    f=open('wrongfile')
    lines=f.readlines()
    f.close()
    # if len(lines)!=1:
    #     return 0

    if os.path.exists('diffwr'):
        exccmd('rm diffwr')
    exccmd('diff wrongfile rightfile > diffwr')
    f=open('diffwr')
    diffmesslines=f.readlines()
    f.close()

    if os.path.exists('diffow'):
        exccmd('rm diffow')
    exccmd('diff wrongfile oriwrongfile > diffow')
    f=open('diffow')
    diffowlines=f.readlines()
    f.close()

    if len(diffmesslines)==0:
        return 1 # pass
    else:
        if len(diffowlines)==0: # 'core dumped' in lines[0]:
            return 2 # still fail
        else:
            return 0