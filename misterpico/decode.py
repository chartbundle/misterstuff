#! /usr/bin/env python3

import sys
import os
import serial
import time
import multiprocessing as mp
import signal
import socket
import select
import queue
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
def picoio(qin,qs):
    pr=cProfile.Profile(time.process_time)
    pr.enable()
    picoio_real(qin,qs)
    pr.disable()
    pr.print_stats()


def picoio_real(qin,qs):
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
                            if (not qs['oled'].full() ):
                                qs['oled'].put_nowait("".join(x))
                    if (e == b'\x50'):
                        length = int.from_bytes(myread(ser,1),'little',signed=False)
                        if (length ==  0 or length > 8):
                            continue
                        data = myread(ser,length)
                        if (len(data) != length):
                            continue
                        for i in range(length):
                            if (not qs['pwm'].full() ):
                                qs['pwm'].put_nowait("PWM:%d %02X"% (i,data[i]))
                else:
                    if (not qs['raw'].full() ):
                        if (d[0] > 127):
                            qs['raw'].put_nowait("$%2X " % (d[0]) )
                        else:
                            qs['raw'].put_nowait(d.decode('ascii'))
                    else:
                        overflow +=1
                        if (overflow % 100 == 0 ):
                            print("Overflow %d" % (overflow))
                    prev=d
        except KeyboardInterrupt:
            sys.exit()
        except Exception as e:
#            print("Main: "+repr(e))
            traceback.print_exc()
            time.sleep(5)

# qin is from tcp to pico
# qout is from pico to tcp
def tcpio(qin,qout,port):
    pr=cProfile.Profile()
    pr.enable()
    tcpio_real(qin,qout,port)
    pr.disable()
    # pr.print_stats()

def tcpio_real(qin,qout,port):
    # Create a TCP/IP socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.setblocking(0)

    # Bind the socket to the port
    server_address = ('localhost', port)
    print ('starting up on %s port %s' % server_address)
    server.bind(server_address)
    # Listen for incoming connections
    server.listen(5)
    inputs = [ server ]
    outputs = []
    tdur = time.time()+60
    while(time.time()<tdur):
        try:
            readable, writable, exceptional = select.select(inputs, outputs, inputs)
            for s in readable:

                if s is server:
                    # A "readable" server socket is ready to accept a connection
                    connection, client_address = s.accept()
                    print('new connection from', client_address)
                    connection.setblocking(0)
                    inputs.append(connection)
                    outputs.append(connection)
                else:
                    try:
                        data=s.recv(1024)
                    except Exception as e:
                        print("Read Exception:" + repr(e))
                    if (data):
                        qin.put(data)
                    else:
                        # Interpret empty result as closed connection
                        print ('closing socket after reading no data')
                        # Stop listening for input on the connection
                        if s in outputs:
                            outputs.remove(s)
                        inputs.remove(s)
                        s.close()
            try:
                
                qd = qout.get(block=True, timeout=0.05)
                # print("QD:"+qd)
                if (type(qd) is str):
                        qd = qd.encode()
                for s in writable:
                    if (s in outputs):
                        try:
                            s.send(qd)
                        except:
                            pass
            except queue.Empty:
                pass
        except Exception as e:
            print("Exc2:"+repr(e))

if __name__ == '__main__':
    qs = {}
    qin = mp.Queue(16)
    qs['oled'] = mp.Queue(2)
    qs['pwm'] = mp.Queue(4)
    qs['raw'] = mp.Queue(32)

    ppico = mp.Process(target=picoio, args=(qin,qs,))
    praw = mp.Process(target=tcpio, args=(qin,qs['raw'],35310))
    poled = mp.Process(target=tcpio, args=(qin,qs['oled'],35311))
    ppwm = mp.Process(target=tcpio, args=(qin,qs['pwm'],35312))
    ppico.start()
    praw.start()
    poled.start()
    ppwm.start()
    try:
        ppico.join()
        ppico.terminate()
        praw.terminate()
        poled.terminate()
        ppwm.terminate()
    except KeyboardInterrupt:
        print("Interrupt")
    ppico.terminate()
    praw.terminate()
    poled.terminate()
    ppwm.terminate()
    ppico.join()
    praw.join()
    poled.join()
    ppwm.join()
    sys.exit()