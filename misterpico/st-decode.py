#! /usr/bin/env python3

import sys
import os
import serial
import time
import signal
import socket
import select
import cProfile
import traceback


def sys_exit(a,b):
    sys.exit()

signal.signal(signal.SIGTERM, sys_exit)


readfifo=bytearray()

def myread(ser,inbytes):
    global readfifo
    inwait=ser.inWaiting()
    if (inwait>0):
        readfifo += ser.read(inwait)
    if (len(readfifo)<inbytes):
        readfifo += ser.read(inbytes-len(readfifo))
    if (len(readfifo)<=inbytes):
        x=readfifo
        readfifo= bytearray()
    else:
        x=readfifo[0:inbytes]
        readfifo=readfifo[inbytes:]
    return x



# qs is a dict of queues used for output
def picoio():
    pr=cProfile.Profile(time.process_time)
    pr.enable()
    picoio_real()
    pr.disable()
    pr.print_stats()

outputs=['raw','oled','pwm']
outport={}
outport['raw']= 35310
outport['oled']= 35311
outport['pwm']= 35312


def picoio_real():
    global outputs
    outdata={}
    for i in outputs:
        outdata[i]=bytearray()

    overflow=0
    tdur=time.time()+65;
    while(time.time()<tdur):
        try:
            prev = b'\x0a'
            try:
                ser = serial.Serial("/dev/ttyACM0",timeout=0.1)
            except:
                try:
                    ser = serial.Serial("/dev/ttyACM1",timeout=0.1)
                except:
                    ser = serial.Serial("/dev/ttyACM2",timeout=0.1)


            state=0
            while(time.time()<tdur):
                try:
                    qd = qin.get_nowait()
                    ser.write(qd)
                except:
                    pass

                d=myread(ser,1)
                if (len(d)==1 and d == b'\x0e'):
                    e = myread(ser,1)
                    if (len(e) == 0 ):
                        continue
                    if (e == b'\x41'):
                        length=int.from_bytes(myread(ser,2),'little',signed=False)
                        if (length == 0 or length > 1024):
                            continue
                        print("in len"+str(length))
                        data=myread(ser,length)
                        if (len(data) < length):
                            continue
                        # decode the data
                        outdata = bytearray(b'')
                        decode=0
                        val=0
                        for b in data:
                            if (decode == 0 and b != 0xd2 ):
                                outdata.append(b)
                            else:
                                if (decode == 2 ):
                                    for i in range(b):
                                        outdata.append(val)
                                    decode = 0
                                elif (decode==1):
                                    val=b
                                    decode=2
                                elif (decode == 0 and b ==  0xd2):
                                    decode=1
                        if (decode == 2 ):
                            for i in range(b):
                                outdata.append(val)
                        # print(bytes(outdata[1:]).hex())
                        # print(len(outdata))
                        fdata = outdata[1:]
                        rows=[]
                        for y in range(64):
                            bit = 2 ** (y % 8)
                            off=128*(y // 8)
                            cols=[]
                            for x in range(128):
                                if (outdata[off+x] & bit):
                                    cols.append('X')
                                else:
                                    cols.append('.')
                            rows.append(cols)

                        for x in rows:
                                outdata['oled'] += ("".join(x))
                    if (e == b'\x50'):
                        length = int.from_bytes(myread(ser,1),'little',signed=False)
                        if (length ==  0 or length > 8):
                            continue
                        data = myread(ser,length)
                        if (len(data) != length):
                            continue
                        for i in range(length):
                            outdata['pwm'] += ("PWM:%d %02X\n"% (i,data[i])).encode('ascii')
                else:
                        if (d[0] > 127):
                            outdata['raw'] += ("$%2X " % (d[0]) ).encode('ascii')
                        else:
                            outdata['raw'] += d
                        prev=d
                indata=bytearray()
                for i in outputs:
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
    tcp[name]['inputs'] = [ tcp[name]['server'] ]
    tcp[name]['outputs'] = []

def tcpio_loop(name,data):
    global tcp
    returndata = bytearray()
    try:
        if (data):
            readable, writable, exceptional = select.select(tcp[name]['inputs'], tcp[name]['outputs'], tcp[name]['inputs'],0)
        else:
            readable, writable, exceptional = select.select(tcp[name]['inputs'], [], tcp[name]['inputs'],0)

        for s in readable:

            if s is tcp[name]['server']:
                # A "readable" server socket is ready to accept a connection
                connection, client_address = s.accept()
                print('new connection from', client_address)
                connection.setblocking(0)
                tcp[name]['inputs'].append(connection)
                tcp[name]['outputs'].append(connection)
            else:
                try:
                    indata=s.recv(1024)
                except Exception as e:
                    print("Read Exception:" + repr(e))
                if (indata):
                    returndata += indata
                else:
                    # Interpret empty result as closed connection
                    print ('closing socket after reading no data')
                    # Stop listening for input on the connection
                    if s in tcp[name]['outputs']:
                        tcp[name]['outputs'].remove(s)
                    tcp[name]['inputs'].remove(s)
                    s.close()
        if (data):
            for s in writable:
                if (s in tcp[name]['outputs']):
                    try:
                        s.send(data)
                    except Exception as e:
                        print("Excsend:"+repr(e))
    except Exception as e:
        print("Exc2:"+repr(e))
    return(returndata)


if __name__ == '__main__':
    for i in outputs:
        tcpio_setup(i,outport[i])
    picoio()
