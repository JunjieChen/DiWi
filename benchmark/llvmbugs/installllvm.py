import os,random
import datetime
import os.path
import subprocess as subp
import sys
import thread

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

revisions=['r213464']
cmakedir='/home/Anonymous/CompilerFL/llvm'
srcdir='/devdata/Anonymous/CompilerFL/llvmforme'
for rev in revisions:
    revpath=srcdir+'/'+rev
    if os.path.exists(revpath):
        exccmd('rm -rf '+revpath)
    os.system('mkdir '+revpath)
    os.system('svn co http://llvm.org/svn/llvm-project/llvm/trunk -'+rev+' '+revpath+'/'+rev)
    os.system('svn co http://llvm.org/svn/llvm-project/cfe/trunk -'+rev+' '+revpath+'/'+rev+'/tools/clang')
    os.system('mkdir '+revpath+'/'+rev+'-build')
    os.chdir(revpath+'/'+rev+'-build')
    os.system(cmakedir+'/cmake/cmake-3.10.0-rc4/bin/cmake -DCMAKE_EXPORT_COMPILER_COMMANDS=ON -DCMAKE_INSTALL_PREFIX='+srcdir+'/'+rev+'/'+rev+'-build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_FLAGS=\"-g -O0 -fprofile-arcs -ftest-coverage\" -DCMAKE_CXX_FLAGS=\"-g -O0 -fprofile-arcs -ftest-coverage\" -DCMAKE_EXE_LINKER_FLAGS=\"-g -fprofile-arcs -ftest-coverage -lgcov\" -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python ../'+rev)
    os.system('make -j 4')
    os.system('make install')
    os.chdir(srcdir)








