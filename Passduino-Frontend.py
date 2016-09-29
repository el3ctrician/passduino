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
        password = raw_input('Enter root password:')

        while :
            if len(password) <= 5:
                print('The inserted password is too short')
            elif len(password) > 10:
                print('The inserted password is too long')
            else:
                break

        serial_port.write(password)

        if serial_port.readall() == 1:
            root = True
            return True

    root = False
    return False

def get_password(uid):
    serial_port.write(uid)
    response = serial_port.read()

    if response == 1:
        pyperclip.copy(serial_port.readall())
        return True

    print('No password associated with this uid')
    return False

def set_password(uid, password):
    serial_port.write(uid)
    serial_port.write(password)

def hard_reset():
    serial_port.write('This is the end, my only friend, the end')

def init():
    serial_port.open()


if __name__ == "__main__":
    init()
    while 1:
        response = serial_port.read()

        if pin_pressed == True:
            go_root(mode)

        serial_port.write(mode)

        if root == True:
            if request == 'setpassword':
                set_password(uid)
            elif request == 'hardreset':
                hard_reset()

        get_password(response)

        root = False
