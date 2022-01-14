import sys
sys.path.append("../")
import etcd3
from etcd.etcd import Etcd_S

def jr_txt2etcd(filename): #[(key,value),(),...]
    filein = open(filename, 'r', encoding='utf-8')
    text = filein.readlines()
    filein.close()
    
    etcd_res = []
    for i in range(0, len(text) // 2):
        etcd_res.append((text[i * 2].strip('\n'), text[i * 2 + 1].strip('\n')))
    
    return etcd_res

def jr_etcd2txt(etcd_all, filename):
    fileout = open(filename, 'w', encoding='utf-8')
    for i in etcd_all:
        try:
            fileout.write(str(i[1].key.decode()) + '\n')
            fileout.write(str(i[0].decode()) + '\n')
        except:
            continue
    fileout.close()

'''
etcd = etcd3.client(host="10.77.70.61", port=2379)
a = etcd.get_all()
jr_etcd2txt(a, 'jr_etcd.txt')

b = jr_txt2etcd('jr_etcd.txt')
for i in b:
    etcd.put(i[0], i[1])

c = etcd.get_all()
jr_etcd2txt(c, 'jr_etcd2.txt')
'''

#'''
etcd = etcd3.client(host="10.77.70.61", port=2379)
a = etcd.get_all()
fileout = open('jr_show_all.txt', 'w', encoding='utf-8')

for i in a:
    print(str(i[1].key.decode()), str(i[0]))
    fileout.write(str(i[1].key.decode()) + ' ' + str(i[0]) + '\n')

fileout.close()

#print(eval(etcd.get("/table/Book/columns")[0]))
#print(eval(etcd.get("/table/Book/fragment/2")[0]))
#'''