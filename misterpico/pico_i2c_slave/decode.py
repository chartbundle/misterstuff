#! /usr/bin/env python3

import sys
import os
import serial
import time
while(1):
    try:
        prev = b'\x0a'
        try:
            ser = serial.Serial("/dev/ttyACM0",timeout=None)
        except:
            try:
                ser = serial.Serial("/dev/ttyACM1",timeout=None)
            except:
                ser = serial.Serial("/dev/ttyACM2",timeout=None)

        while(1):
            d=ser.read(1)
            if (d == b'\x0e'):
                e = ser.read(1)
                if (e == b'\x50'):
                    length=int.from_bytes(ser.read(2),'little',signed=False)
                    print("in len"+str(length))
                    data=ser.read(length)
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
                        print("".join(x))

                    prev= b'\x0a'
            else:
                print(d.decode('ascii'),end='')
                prev=d
            sys.stdout.flush()
    except KeyboardInterrupt:
        sys.exit()
    except Exception as e:
        print("Main Loop:"+str(e))
        time.sleep(5)
