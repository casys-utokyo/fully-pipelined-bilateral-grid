import os
import subprocess
from subprocess import PIPE

def main():
    cd = os.path.dirname(os.path.abspath(__file__))
    cpppath = os.path.join(cd, "header_template.cpp")
    headerpath = os.path.join(cd, "bilateral_grid.h")

    gpp = subprocess.Popen("g++  -Wall -std=c++11 "+cpppath, shell=True, stdout=PIPE, stderr=PIPE)
    errs = [err.decode() for err in gpp.stderr.readlines()]
    err_idx = 1
    err_out = ""
    while err_idx < len(errs):
        if "warning: ISO C++ forbids converting a string constant to ‘char*’" in errs[err_idx]:
            err_idx += 6
        else:
            err_out += errs[err_idx]
            err_idx += 1
    if err_out != "":
        print(errs[0] + err_out)
        if gpp.wait() == 1:
            return

    aout = subprocess.Popen("./a.out", shell=True, stdout=PIPE, stderr=PIPE)
    errs = [err.decode() for err in aout.stderr.readlines()]
    if len(errs) >= 1:
        for err in errs:
            print(err[:-1])
        if aout.wait() == 1:
            return

    rm = subprocess.Popen("rm ./a.out", shell=True, stdout=PIPE, stderr=PIPE)
    errs = [err.decode() for err in rm.stderr.readlines()]
    if len(errs) >= 1:
        for err in errs:
            print(err[:-1])
        if rm.wait() == 1:
            return

    vals = [s.decode().split() for s in aout.stdout.readlines() if s != '']

    with open(cpppath, mode='r') as fi:
        with open(headerpath, mode='w') as fo:
            v_idx = 0
            for l in fi:
                if l.startswith("//") and "ignore above" in l:
                    break
            for l in fi:
                if l != '':
                    break
            for l in fi:
                if l.startswith("//"):
                    if "ignore below" in l:
                        return v_idx, vals
                    elif "ignore" in l:
                        continue
                    elif "start lists:" in l:
                        if vals[v_idx][0] == "LISTS:":
                            v_idx += 1
                            break
                    else:
                        tmp_idx = 2
                        while (l[tmp_idx] == ' '):
                            tmp_idx += 1
                        fo.write(l[tmp_idx:])
                else:
                    ll = l.split()
                    if len(l) > 3 and v_idx < len(vals) and ll[0] == "const" and ll[2] == vals[v_idx][0]:
                        fo.write(' '.join(ll[:4] + [vals[v_idx][1] + ';\n']))
                        v_idx += 1
                    else:
                        fo.write(l)
            for l in fi:
                if l.startswith("//"):
                    if "ignore below" in l:
                        return v_idx, vals
                    elif "ignore" in l:
                        continue
                    else:
                        tmp_idx = 2
                        while (l[tmp_idx] == ' '):
                            tmp_idx += 1
                        fo.write(l[tmp_idx:])
                else:
                    ll = l.split()
                    if len(l) > 3 and v_idx < len(vals) and ll[0] == "const" and ll[2] == vals[v_idx][0]:
                        fo.write(' '.join(ll[:2] + [ll[2]+'[' + vals[v_idx][1] + ']'] + ['='] + vals[v_idx+1]))
                        fo.write(';\n')
                        v_idx += 2
                    else:
                        fo.write(l)
    return v_idx, vals


def output(v_idx, vals):
    conv_vals = "Converted variables:"
    if v_idx > 0:
        conv_vals = ' '.join([conv_vals, vals[0][0]])
        for i in range(1, v_idx):
            if not('LISTS:' in vals[i][0]) and not('{' in vals[i][0]):
                conv_vals = ", ".join([conv_vals, vals[i][0]])
    else:
        conv_vals = ' '.join([conv_vals, "None"])

    print(conv_vals)
    if v_idx < len(vals):
        print("Converted variables are less than declared")
    else:
        print("Succeed")


if __name__ == '__main__':
    v_idx, vals = main()
    output(v_idx, vals)