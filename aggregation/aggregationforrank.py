import os,random,math
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
basicdir='/devdata/Anonymous/CompilerFL/llvmforme/locations'
passcovdir='/devdata/Anonymous/CompilerFL/llvmforme/result/MCMC_new'
locationdir='/devdata/Anonymous/CompilerFL/llvmforme/locations'

revisions=[]
revfile=open('llvmbugs_summary.txt')
revlines=revfile.readlines()
revfile.close()
for i in range(len(revlines)):
	revisions.append(revlines[i].strip().split(',')[1])
#'r196310','r198967','r199531','r199601','r199849','r200388','r201397','r201915','r201986','r202067','r202308','r202556','r202643','r202882','r203003','r203223','r203235','r203511',
result=open('FileRank.csv','w')
for rev in revisions:
	# if not os.path.exists(passcovdir+'/'+rev):
	# 	continue
	result.write(rev+':\n')
	locationfile=open(locationdir+'/'+rev[1:]+'/locations')
	locationlines=locationfile.readlines()
	locationfile.close()
	buggyfiles=set()
	for i in range(len(locationlines)):
		if 'file' in locationlines[i].strip() and 'method' in locationlines[i].strip():
			buggyfile='lib/'+locationlines[i].strip().split(';')[0].strip().split(':')[1].strip().split('/lib/')[1]
			buggyfiles.add(buggyfile)

	if os.path.exists(basicdir+'/'+rev[1:]+'/failcov/stmt_info.txt'):
		tarpath=basicdir+'/'+rev[1:]+'/failcov/stmt_info.txt'
	elif os.path.exists(basicdir+'/'+rev[1:]+'/fail/stmt_info.txt'):
		tarpath=basicdir+'/'+rev[1:]+'/fail/stmt_info.txt'
	else:
		print "Error!!"
		sys.exit(1)

	failfile=open(tarpath)
	faillines=failfile.readlines()
	failfile.close()

	failstmt=dict()
	passstmt=dict()
	failfileset=set()
	failfilemapstmt=dict()
	for i in range(len(faillines)):
		faillinesplit=faillines[i].strip().split(',')
		filename=faillinesplit[0].strip().split('.gcda')[0].strip()
		if not filename.endswith('.cpp'):
			continue
		failfileset.add(filename)
		stmtlist=faillines[i].strip().split(':')[1].split(',')
		failfilemapstmt[filename]=set(stmtlist)
		for stmt in stmtlist:
			failstmt[filename+','+stmt]=1
			passstmt[filename+','+stmt]=0

	for i in os.listdir(passcovdir+'/'+rev+'/passcov'):
		passfile=open(passcovdir+'/'+rev+'/passcov/'+i+'/stmt_info.txt')
		passlines=passfile.readlines()
		passfile.close()
		for j in range(len(passlines)):
			passlinesplit=passlines[j].strip().split(',')
			filename=passlinesplit[0].strip().split('.gcda')[0].strip()
			if not filename.endswith('.cpp'): # consider c and h files
				continue
			if filename not in failfileset:
				continue
			stmtlist=passlines[j].strip().split(':')[1].split(',')
			for stmt in set(stmtlist)&failfilemapstmt[filename]:
				# if filename+','+stmt in failstmt.keys():
				passstmt[filename+','+stmt]+=1

	score=dict()
	filescore=dict()
	for key in failstmt.keys():
		score[key]=float(failstmt[key])/math.sqrt(float(failstmt[key])*(failstmt[key]+passstmt[key]))
		# if passstmt[key]==0:
		# 	score[key]=1.0
		# else:
		# 	score[key]=float(failstmt[key])/passstmt[key]
		keyfile=key.split(',')[0]
		if keyfile not in filescore.keys():
			filescore[keyfile]=[]
			filescore[keyfile].append(score[key])
		else:
			filescore[keyfile].append(score[key])

	fileaggstmtscore=dict()
	for key in filescore.keys():
		fileaggstmtscore[key]=float(sum(filescore[key]))/len(filescore[key])

	scorelist=sorted(fileaggstmtscore.items(),key=lambda d:d[1],reverse=True)

	for bf in buggyfiles:
		for i in range(len(scorelist)):
			setbf=set(bf.split('/'))
			seti=set(scorelist[i][0].split('/'))
			if setbf.issubset(seti):
				bf=scorelist[i][0]
				break
	# for bf in buggyfiles:
		tmp=[]
		for i in range(len(scorelist)):
			if fileaggstmtscore[bf]==scorelist[i][1]:
				tmp.append(i)
		result.write(bf+','+str(min(tmp)+1)+','+str(max(tmp)+1)+','+str(fileaggstmtscore[bf])+'\n')
	result.write('\n')
	result.flush()
result.close()





