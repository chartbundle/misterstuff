#! /usr/bin/env python3

import sys
import os
import io
import serial
import time
import signal
import socket
import select
import cProfile
import traceback
import pstats
from collections import deque

def sys_exit(a,b):
    sys.exit()

signal.signal(signal.SIGTERM, sys_exit)


readfifo=bytearray()
def myread(ser,inbytes):
    global readfifo
    x=bytearray()
    l = len(readfifo)
    inwait=ser.inWaiting()
    if ((inwait + l) < inbytes ):
        inwait = inbytes
    # if (inwait>0):
    readfifo.extend(ser.read(inwait))
    # if (len(readfifo)<inbytes):
    #     readfifo.extend(ser.read(inbytes-len(readfifo)))
    # if (len(readfifo)<=inbytes):
    #     x=readfifo
    #     readfifo= bytearray()
    # else:
    #     x=readfifo[0:inbytes]
    #     readfifo=readfifo[inbytes:]
    try:
        if (inbytes == 1 ):
            x=bytearray([readfifo.pop(0)])
        else:
            x=bytearray([readfifo.pop(0) for _i in range(inbytes)])
    except:
        pass
    return x



outputs=['raw','oled','pwm']
outport={}
outport['raw']= 35310
outport['oled']= 35311
outport['pwm']= 35312
outtime = {}
outtime['raw'] = 5
outtime['oled'] = 5
outtime['pwm'] = 5
outaggr = {}
outaggr['raw'] = True
outaggr['oled'] = False
outaggr['pwm'] = False

def picoio():
    pr=cProfile.Profile(time.process_time)
    pr.enable()
    picoio_real()
    pr.disable()
    s = io.StringIO()
    sortby = 'tottime'
    ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats()
    print(s.getvalue())

def picoio_real():
    global outputs
    global outtime
    outdata = {}
    lasttime={}
    t = time.time_ns()/1000000
    for i in outputs:
        outdata[i]=bytearray()
        lasttime[i]=t

    overflow=0
    tdur=t+65000
    while(1):
        try:
            try:
                ser = serial.Serial("/dev/ttyACM0",timeout=0.05)
            except:
                try:
                    ser = serial.Serial("/dev/ttyACM1",timeout=0.05)
                except:
                    ser = serial.Serial("/dev/ttyACM2",timeout=0.05)


            state=0
            while(1):
                try:
                    qd = qin.get_nowait()
                    ser.write(qd)
                except:
                    pass

                d=myread(ser,1)
                if (d == b'\x0e'):
                    e = myread(ser,1)
                    if (e == b'\x41'):
                        length=int.from_bytes(myread(ser,2),'little',signed=False)
                        if (length == 0 or length > 1024):
                            continue
                        print("in len"+str(length))
                        data=myread(ser,length)
                        if (len(data) < length):
                            print("Short read: %d"%(len(data)))
                            continue
                        # decode the data
                        oleddata = bytearray(b'')
                        decode=0
                        val=0
                        for b in data:
                            if (decode == 0 and b != 0xd2 ):
                                oleddata.append(b)
                            else:
                                if (decode == 2 ):
                                    for i in range(b):
                                        oleddata.append(val)
                                    decode = 0
                                elif (decode==1):
                                    val=b
                                    decode=2
                                elif (decode == 0 and b ==  0xd2):
                                    decode=1
                        if (decode == 2 ):
                            for i in range(b):
                                oleddata.append(val)
                        #print(bytes(outdata[1:]).hex())
                        print(len(oleddata))
                        #fdata = oleddata[1:]
                        rows=[]
                        for y in range(64):
                            bit = 2 ** (y % 8)
                            off=128*(y // 8)
                            cols=[]
                            for x in range(128):
                                if (oleddata[off+x] & bit):
                                    cols.append('X')
                                else:
                                    cols.append('.')
                            rows.append(cols)

                        for x in rows:
                                outdata['oled'] += (("".join(x))+"\n").encode('ascii')
                    elif (e == b'\x50'):
                        length = int.from_bytes(myread(ser,1),'little',signed=False)
                        if (length ==  0 or length > 8):
                            continue
                        data = myread(ser,length)
                        if (len(data) != length):
                            continue
                        outdata['pwm'] += b'\x50'+ bytearray([length])
                        outdata['pwm'] += data
                else:
                        if (d[0] > 127):
                            outdata['raw'] += ("$%2X " % (d[0]) ).encode('ascii')
                        else:
                            outdata['raw'] += d
                indata=bytearray()
                t = time.time_ns() / 1000000
                for i in outputs:
                    ck=False
                    # aggregate outdata if outaggr otherwise push immediately on new data
                    if ( (not outaggr[i]) and outdata[i] ):
                        ck = True
                    if (ck or (lasttime[i]+ outtime[i])<t ):
                        lasttime[i]=t
                        indata += tcpio_loop(i,outdata[i])
                        outdata[i]=bytearray()
                if (indata):
                    ser.write(indata)


        except KeyboardInterrupt:
            sys.exit()
        except Exception as e:
#            print("Main: "+repr(e))
            traceback.print_exc()
            time.sleep(5)

tcp = {}
def tcpio_setup(name,port):
    # Create a TCP/IP socket
    global tcp
    tcp[name]={}
    tcp[name]['server']= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp[name]['server'].setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    tcp[name]['server'].setblocking(0)

    # Bind the socket to the port
    server_address = ('localhost', port)
    print ('starting up on %s port %s' % server_address)
    tcp[name]['server'].bind(server_address)
    # Listen for incoming connections
    tcp[name]['server'].listen(5)
    tcp[name]['epoll'] = select.epoll()
    tcp[name]['epoll'].register(tcp[name]['server'].fileno(),select.EPOLLIN )
    tcp[name]['conn'] = {}

def tcpio_loop(name,data):
    global tcp
    returndata = bytearray()
    try:
        e = tcp[name]['epoll'].poll(0)

        for fn,ev in e:

            if fn == tcp[name]['server'].fileno():
                # A "readable" server socket is ready to accept a connection
                connection, client_address = tcp[name]['server'].accept()
                print('new connection from', client_address)
                connection.setblocking(0)
                tcp[name]['conn'][connection.fileno()] = connection
                tcp[name]['epoll'].register(connection.fileno(),select.EPOLLIN | select.EPOLLOUT)
            else:
                if ev & (select.EPOLLHUP | select.EPOLLRDHUP | select.EPOLLERR): 
                    print ('closing socket after HUP')
                    # Stop listening for input on the connection
                    tcp[name]['epoll'].unregister(fn)
                    tcp[name]['conn'][fn].close()
                    del tcp[name]['conn'][fn]
                else:
                    if  ev & select.EPOLLIN:
                        try:
                            indata=tcp[name]['conn'][fn].recv(1024)
                        except Exception as e:
                            print("Read Exception:" + repr(e))
                        if (indata):
                            returndata += indata
                    if (data and ev & select.EPOLLOUT):
                        try:
                            tcp[name]['conn'][fn].send(data)
                        except Exception as e:
                            print("Excsend:"+repr(e))
    except Exception as e:
        print("Exc2:"+repr(e))
    return(returndata)


if __name__ == '__main__':
    for i in outputs:
        tcpio_setup(i,outport[i])
    picoio()
