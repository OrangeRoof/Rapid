import serial_manager_plus
import tp_manager
import logging
import datetime
import json

# of format [('port', function)...]
gets = []

def serial_gui_factory(serial, tp, period=5):
    def get():
        port = serial.name
        l = tp.process(serial.readline())
        logging.info('{}: {}'.format(tp.__name__, l))
        return(l)
    # tuple factory. Inputs a serial object and a tp object and the frequency
    # creates a function that reads from the serial and processes in the tp, 
    # logs it. assumes that a logfile was created before (logging.basicConfig)
    return get

def start_serial(args):
    # Expects input of the type [('serial', 'tp') ...]
    # NOTICE THE STRING TYPES
    # Open tp ports,
    # input: the form data from /config's submit.
    # sanitizes and converts form into a list,
    # then filters list for serial/tp pairs.
    # opens, builds functions, and adds to gets

    global gets
    # filter args [('name', 'tp')] for tuples where 'tp' is something
    args = list(filter(lambda n: n[1], args))
    tp_manager.import_tp()
    logging.basicConfig(filename=str(datetime.datetime.now()),
                        format='%(asctime)s %(message)s',
                        datefmt='%I:%M:%S',
                        level=logging.INFO)
    for stp in args:
        # serial at stp[0], tp at stp[1]
        serial_manager_plus.open(stp[0]) 
        gets.append((
            stp[0],
            serial_gui_factory(
                serial_manager_plus.get_serial(stp[0]),
                tp_manager.get_tp(stp[1]))))


def read(port):
    # input: str port location ('COM1' or '/dev/tty.L')
    # goes through all tuples in get and compare names 
    # if match found, calls get() (g[1]) and returns list output
    # the idea is the frontend js controls refresh rate:
    # user calls the refresh every x seconds, js GETs /read, server does least
    global gets
    for g in gets:
        if g[0] == port:
            return(json.dumps(g[1]()))
    raise Exception('Open port matching {} not found!'.format(port))
