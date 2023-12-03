#!/usr/bin/python3

import hid
import time
import os
from struct import pack,unpack

BUFSIZ=63

#print(hid.enumerate())

h = hid.device()
h.open(0xfeed, 0xc0de)
print("opened %s" % h.get_product_string())

def send_cmd(cmd, param):
    h.write(pack("<h%dsh%ds" % (len(cmd)+1,len(param)+1,), len(cmd)+1, bytes(cmd+'\0', "utf-8"), len(param)+1,  bytes(param+'\0', "utf-8")))

def do_ls(arg):
    send_cmd("ls", arg)

    while(True):

        b = h.read(64)
        l = len(b) - 1

        if l <= 0:
            break

        s = unpack("%ds" % (l,), bytes(b[1:l+1]))[0].decode("UTF-8");
        print(s, end = "\t")

        send_cmd("next", "")

    print("")

def do_get(arg):
    byteCount = 0;
    send_cmd("get", arg)
    time.sleep(0.1)
    f = open(arg, "wb")

    while(True):

        b = h.read(64)
        l = len(b) - 1
        byteCount += l

        if l <= 0:
            break

        s = unpack("%ds" % (l,), bytes(b[1:l+1]))[0];

        f.write(s)
        send_cmd("next", "")

    f.close()

    print("%d bytes received" % byteCount)

def do_put(arg):
    byteCount = 0;
    send_cmd("put", arg)
    time.sleep(0.1)
    f = open(arg, "rb")

    while(True):

        b = f.read(BUFSIZ)

        if len(b) > 0:
            byteCount+=h.write(b)
        else:
            h.write([0])
            break    

    f.close()

    print("%d bytes sent" % byteCount)

def do_lls(arg):

    dirlist = os.listdir(arg)

    for entry in dirlist:
        print(entry, end=" ")

    print("")
    return

def do_help():
    print("Commands are:\n")
    print("?\tget\tls\tlls\tput\tquit")

while(True):

    print("hidftp> ", end="")
    ins = input()

    cmd = ins.split(" ")

    if cmd[0] == "ls":
        do_ls(cmd[1] if len(cmd) > 1 else ".")
    elif cmd[0] == "lls":
        do_lls(cmd[1] if len(cmd) > 1 else ".")
    elif cmd[0] == "get":
        if(len(cmd) > 1):
            do_get(cmd[1])
        else:
            print("Missing filename argument")
    elif cmd[0] == "put":
        if(len(cmd) > 1):
            do_put(cmd[1])
        else:
            print("Missing filename argument")
    elif cmd[0] == "quit":
        exit(0)
    elif cmd[0] == "?":
        do_help()
    else:
        print("Invalid command")

