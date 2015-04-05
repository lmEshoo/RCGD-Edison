#!/usr/bin/python

from __future__ import absolute_import, print_function, unicode_literals

from optparse import OptionParser, make_option
import os
import sys
import socket
import uuid
import dbus
import dbus.service
import mraa
#http://iotdk.intel.com/docs/master/mraa/python/example.html

import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

PIN_MAP = { 'BACK_MOTOR':mraa.Gpio(12), 'FRONT_MOTOR':mraa.Gpio(13), 'BACK_BRAKE':mraa.Gpio(9), 'FRONT_BRAKE': mraa.Gpio(8) }

for key, value in PIN_MAP:
    value.dir(mraa.DIR_OUT)

pwmA = mraa.Pwm(3)
pwmA.period_us(700)
pwmA.enable(True)

pwmB = mraa.Pwm(11)
pwmB.period_us(700)
pwmB.enable(True)

XBOX_MAP = {
    0x0E : 'LEFT_STICK'

}

def getIntFromBytes(b):
    a = ''
    for i in range(0,4):
        c = '{0:b}'.format(b[i])
        for d in range(4 - len(c)):
            c = '0' + c
        a += c
    return int(a,2)


class Profile(dbus.service.Object):
    fd = -1

    @dbus.service.method("org.bluez.Profile1",
                    in_signature="", out_signature="")
    def Release(self):
        print("Release")
        mainloop.quit()

    @dbus.service.method("org.bluez.Profile1",
                    in_signature="", out_signature="")
    def Cancel(self):
        print("Cancel")

    @dbus.service.method("org.bluez.Profile1",
                in_signature="oha{sv}", out_signature="")
    def NewConnection(self, path, fd, properties):
        self.fd = fd.take()
        print("NewConnection(%s, %d)" % (path, self.fd))


        server_sock = socket.fromfd(self.fd, socket.AF_UNIX, socket.SOCK_STREAM)
        server_sock.setblocking(1)
        server_sock.send("This is Edison SPP loopback test\nAll data will be loopback\nPlease start:\n")

        try:
            while True:
                data = [None] * 1024
                nBytes = server_sock.recv_into(data)

                if(nBytes > 10): print "Too many bytes!"
                
                if(data[0] != 2): 
                    print 'Start Bit Incorrect'
                    raise IOError

                if(data[nBytes - 1] != 3):
                    print 'End Bit Incorrect'
                    raise IOError


                key = getIntFromBytes(data[1:5])
                value = getIntFromBytes(data[5:9])

                xbox = XBOX_MAP[key]

                #LEFT_STICK is forward and back
                #RIGHT_STICK is left and right
                print("received: %d" % nBytes)
                    
                normalizedValue = ((value + 32768)/(32768.0 + 32767.0))
                if(xbox == 'LEFT_STICK'):
                    if(value > 100):
                        #FORWARD
                        PIN_MAP['BACK_BRAKE'].write(0)
                        PIN_MAP['FRONT_MOTOR'].write(1)
                        PIN_MAP['BACK_MOTOR'].write(1)
                        pwmA.write(normalizedValue)#whatever the value is 
                    elif(value < -100):
                        #BACKWARD
                        PIN_MAP['BACK_BRAKE'].write(0)
                        pwmA.write(normalizedValue)
                        PIN_MAP['BACK_MOTOR'].write(0)
                    else:
                        #STOP
                        PIN_MAP['BACK_BRAKE'].write(1)
                        PIN_MAP['FRONT_BRAKE'].write(1)

                elif(xbox == 'RIGHT_STICK')
                    #GO RIGHT
                    if(value > 100):
                        PIN_MAP['FRONT_BRAKE'].write(0)
                        PIN_MAP['FRONT_MOTOR'].write(1)
                        pwmB.write(normalizedValue)
                        PIN_MAP['BACK_BRAKE'].write(0)
                        PIN_MAP['BACK_MOTOR'].write(1)
                        pwmA.write(normalizedValue)
                    #GO LEFT
                    else if (value < -100): #-100?
                        PIN_MAP['FRONT_BRAKE'].write(0)
                        PIN_MAP['FRONT_MOTOR'].write(0)
                        pwmB.write(1-normalizedValue) #255 being max
                        PIN_MAP['BACK_BRAKE'].write(0)
                        PIN_MAP['BACK_MOTOR'].write(1)
                        pwmA.write(normalizedValue)
                #print("received: %s" % data)
            #server_sock.send("looping back: %s\n" % data)
        except IOError:
            pass

        server_sock.close()
        print("all done")



    @dbus.service.method("org.bluez.Profile1",
                in_signature="o", out_signature="")
    def RequestDisconnection(self, path):
        print("RequestDisconnection(%s)" % (path))

        if (self.fd > 0):
            os.close(self.fd)
            self.fd = -1

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    bus = dbus.SystemBus()

    manager = dbus.Interface(bus.get_object("org.bluez",
                "/org/bluez"), "org.bluez.ProfileManager1")

    option_list = [
            make_option("-C", "--channel", action="store",
                    type="int", dest="channel",
                    default=None),
            ]

    parser = OptionParser(option_list=option_list)

    (options, args) = parser.parse_args()

    options.uuid = "1101"
    options.psm = "3"
    options.role = "server"
    options.name = "Edison SPP Loopback"
    options.service = "spp char loopback"
    options.path = "/foo/bar/profile"
    options.auto_connect = False
    options.record = ""

    profile = Profile(bus, options.path)

    mainloop = GObject.MainLoop()

    opts = {
            "AutoConnect" : options.auto_connect,
        }

    if (options.name):
        opts["Name"] = options.name

    if (options.role):
        opts["Role"] = options.role

    if (options.psm is not None):
        opts["PSM"] = dbus.UInt16(options.psm)

    if (options.channel is not None):
        opts["Channel"] = dbus.UInt16(options.channel)

    if (options.record):
        opts["ServiceRecord"] = options.record

    if (options.service):
        opts["Service"] = options.service

    if not options.uuid:
        options.uuid = str(uuid.uuid4())

    manager.RegisterProfile(options.path, options.uuid, opts)

    mainloop.run()
