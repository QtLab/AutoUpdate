# coding=utf-8
import os,time,datetime,math,sys,json,re
import codecs

version = input("input version:")

c = os.getcwd()

c.strip().lstrip().rstrip(',')

n = len(c)+1

g = os.walk(c)

lst = []

for path,d,filelist in g:
    for filename in filelist:
        if filename == "GenerateFileInfo.py"  or filename == "AutoUpdate.exe" or filename == "config.json":
            continue
        f = os.path.join(path, filename)
        n1 = len(f) - len(filename)-1
        path_t =f[n:n1]
        path_t = path_t.replace("\\","/")
        time=math.floor(os.path.getmtime(f))
        time = str(time)
        info={"time":time,"path":path_t}
        file = {filename:info}
        lst.append(file)
info = {"CurrentVersion":version,"Files":lst}

#out = json.dumps(info,ensure_ascii=False,sort_keys=True,indent=2,encoding='utf-8')
out = json.dumps(info,ensure_ascii=False,sort_keys=True,indent=2)

print(out)
f = open('configUpdate.txt','w')
f.truncate()
#f.write(out.encode('utf-8'))
f.write(out)
f.close()

raw_input("Press<enter>")
