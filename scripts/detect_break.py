import argparse, sys

def find_breaks(opts):
    min_rat = opts.min_rat
    if opts.fin is None:
        fin = sys.stdin
    else:
        fin = open(opts.fin)
    
    infor = []
    for ln in fin:
        lnlist = ln.strip().split()
        infor.append(lnlist)
    linfor = len(infor)
    i = 0
    while i < linfor:
        if infor[i][3] == "gap":
            if float(infor[i][5]) / float(infor[i-1][5]) <= min_rat and float(infor[i][5])/ float(infor[i+1][5]) <= min_rat:
                print ("{0}\t{1}".format(infor[i-1][3], infor[i+1][3]))
            # a = float(infor[i][5]) * float (infor[i+1][4])/(2 * float(infor[i+1][5]) * float(infor[i-1][4]))
            # b = float(infor[i][5]) * float (infor[i-1][4])/(2 * float(infor[i-1][5]) * float(infor[i+1][4]))
            # a = float(infor[i][5]) / float(infor[i-1][5]) 
            # b = float(infor[i][5])/float(infor[i+1][5])
            # e = float(infor[i][5]) / float(infor[i-1][4])/float(infor[i+1][4])
            # if e < 2e-9:
                # if a < 0.3 and b < 0.3:
                    # print ("B\t"+infor[i-1][3])
                # elif a < 0.9 or b < 0.9:
                    # print ("S\t"+ infor[i-1][3])
                # print (infor[i-1][3])
            # if a > b:
                # t = a 
                # a = b
                # b = t
            # print (str(a*100) + '\t' + str(b*100) + '\t' + str(e) + '\t' + '\t'.join(infor[i]))
        i += 1
    
    if opts.fin is not None:
        fin.close() 

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Break finder')
    parser.add_argument('-m', '--min_rat', type = float, action = "store", dest = "min_rat", help = 'minimum ratio between maximum coverage and gap coverage [0.2]', default=0.2)
    parser.add_argument('-i', '--input', type = str, dest = "fin", help = 'input coverage stat file [stdin]')
    # parser.add_argument('--version', action='version', version='%(prog)s 0.0.0')
    opts = parser.parse_args()
    find_breaks(opts)
