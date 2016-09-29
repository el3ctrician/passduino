#!/usr/bin/python
import pyperclip, serial

request_modes = {1: "getpasswd", 2: "setpassword", 3: "hardreset"}

serial_port = serial.Serial(
    port='/dev/ttyUSB1',
    baudrate=9600,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

root = False

def go_root(mode):
    serial_port.write(mode)

    if serial_port.readall() == 1:
        root = True


def request(mode, uid):
    serial_port.open()
    serial_port.write(mode)
    response = serial_port.read()

    if response == 1:
        if request == "getpasswd":
            get_password(uid)
        elif request == "setpassword":
            set_password(uid, password)
        else:
            hard_reset()

    if response == 1:
        return True
    else:
        return False


def get_password(uid):
    serial_port.write(uid)
    response = serial_port.read()

    if response == 1:
        return serial_port.readall()

    print('No password associated with this uid')
    return False

def set_password(uid, password):
    serial_port.write(uid)
    serial_port.write(password)

def hard_reset():
    serial_port.write('This is the end, my only friend, the end')

while 1:
    #1)Controlla rfid
    #2)Controlla tastino
    #3)Controlla autodistruzione
