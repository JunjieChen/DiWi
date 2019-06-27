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

revisions=['r210888']

srcdir='/devdata/Anonymous/CompilerFL/gccforme'

for rev in revisions:
    revpath=srcdir+'/'+rev
    if os.path.exists(revpath):
        os.system('rm -rf '+revpath)
    os.system('mkdir '+revpath)
    os.system('svn co svn://gcc.gnu.org/svn/gcc/trunk -'+rev+' '+revpath+'/'+rev)
    os.system('mkdir '+revpath+'/'+rev+'-build')
    os.chdir(revpath+'/'+rev+'-build')

    os.system(revpath+'/'+rev+'/configure --enable-languages=c,c++ --disable-werror --enable-checking=release --with-gmp=/devdata/Anonymous/Anonymous/gmp-4.3.2 --with-mpfr=/devdata/Anonymous/Anonymous/mpfr-3.1.4 --with-mpc=/devdata/Anonymous/Anonymous/mpc-1.0.3 --prefix='+revpath+'/'+rev+'-build'+' --enable-coverage')

    os.system('make -j 16')
    os.system('make install')
    os.chdir(srcdir)



