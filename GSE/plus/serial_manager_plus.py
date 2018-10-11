import serial
import serial.tools.list_ports as list_ports
import config
import sys
#import zmq

# the open ports are exposed as a global variable
current_open_ports = []

def open(port):
    # initialize_ports takes a list of ports ('/dev/arduino', 'COM1', etc.) and tries to open a serial connection to each and every one of them.
    # going through x in ports:
        # if x is a string: assume that its a com port, attempt to open it
        # if x is a tuple: check that it is a tuple of length 2 (c, b)
            # c is the port name (String), b is the baudrate (int)

    global current_open_ports

    # if a single port string was passed in, just add the port
    if type(port) == str:
        try:
            current_open_ports.append(serial.Serial(port, config.BAUDRATE, timeout=config.TIMEOUT, write_timeout=config.TIMEOUT))
        except:
            raise
        return
    # else, assume that a list of ports was passed in and iterate through
    for p in port:
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
            current_open_ports.append(serial.Serial(c, b, timeout=config.TIMEOUT, write_timeout=config.TIMEOUT))
        except:
            raise

def get_ports():
    # serial.tools.list_ports.comports returns list of ports as Info objects
    # Info.device is a string describing the port of the device
    return [p.device for p in list_ports.comports()]

def read():
    # given current open serials, return (for each serial):
    # the line of data in the serial buffer, as well as the name of the port
    # zip merges the two lists into tuples with 2 elements each
    global current_open_ports
    d = [p.readline() for p in current_open_ports]
    return list(zip(d, [s.name for s in current_open_ports]))

def write(port, msg):
    # writes message to serial port chosen.
    # port is the name of the port itself, msg can be anything
    global current_open_ports
    p = list(filter(lambda p: p.device == port, current_open_ports))
    if len(p) == 0:
        raise(Exception('Serial port {} not found in open ports'.format(port)))

    try:
        p[0].write(msg)
    except:
        raise
#no zmq
#def initalize_zmq(telem_addr, command_addr):
#    # open itself as a publisher to the telemetry server
#    # open self as reply in  request/reply pair to the command sender
#    # return the zmq socket objects
#    try:
#        context = zmq.Context()
#        t = context.socket(zmq.PUB)
#        t.bind(server_addr)
#
#        c = context.socket(zmq.REP)
#        c.bind(command_addr)
#    except:
#        raise
#    return(t, c)

# def close_zmq(), close_ports()
    # zmq and serial get closed upon garbage collection
