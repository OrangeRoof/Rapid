import serial
import config
import sys
import zmq
import time

def initialize_ports(ports):
    # initialize_ports takes a list of ports ('/dev/arduino', 'COM1', etc.) and tries to open a serial connection to each and every one of them.
    # going through x in ports:
        # if x is a string: assume that its a com port, attempt to open it
        # if x is a tuple: check that it is a tuple of length 2 (c, b)
            # c is the port name (String), b is the baudrate (int)

    s = []

    for p in ports:
        if type(p) is str:
            c, b = p, config.BAUDRATE
        elif type(p) is tuple:
            if not (len(p) == 2 and type(p[0]) is str and type(p[1]) is int):
                raise(TypeError('List of serial ports contains a tuple of incorrect format {}'.format(p)))
            c, b = p[0], p[1]
        else:
            raise(TypeError('List of serial ports contains unknown type {}'.format(type(p))))
        # add serial device at comport, baudrate, with respective timeouts
        try:
            s.append(serial.Serial(c, b, timeout=config.TIMEOUT, write_timeout=config.TIMEOUT))
        except:
            raise
    return s

def close_ports():
    # close the serial ports when it needs to happen
    pass

def initalize_zmq(server_addr, command_addr):
    # initializes zmq to send data to data processing server, and recieve from commander
    try:
        context = zmq.Context()
        self.server_socket = context.socket(zmq.PUB)
        self.server_socket.bind(server_addr)

        self.command_socket = context.socket(zmq.SUB)
        self.command_socket.connect(command_addr)
        self.command_socket.setsockopt_string(zmq.SUBSCRIBE, "")
    except:
        raise

def close_zmq():
    # close zmq sockets when it needs to happen
    pass
