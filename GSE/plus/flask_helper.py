import serial_manager_plus
import tp_manager
import threading

open_get = []

def serial_gui_factory(serial, tp, period=5):
    def get():
        return(tp.process(serial.readline()))
    # tuple factory. Inputs a serial object and a tp object and the frequency
    # creates a function that reads from the serial and processes in the tp, 
    # then binds it to a Timer object that runs every period seconds
    return get

def start_serial(args):
    global open_get
    tp_manager.import_tp()
    # Expects input of the type [('serial', 'tp') ...]
    # Open tp ports,
    # input: the form data from /config's submit.
    # sanitizes and converts form into a list,
    # then filters list for serial/tp pairs.
    # returns list of get() functions.
    for stp in args:
        # serial at stp[0], tp at stp[1]
        serial_manager_plus.open(stp[0]) 
        open_get.append(serial_gui_factory(serial_manager_plus.current_open_ports[-1]), tp_manager.get_tp(stp[1])) 

def read(port):
    # input: str port location ('COM1' or '/dev/tty.L')
    # goes through all get() functions and compares port to __str__.
    # if match found, calls get() and processes
    # the idea is the frontend js controls refresh rate:
    # user calls the refresh every x seconds, js GETs /read
    # every x seconds, its processed every x seconds.
    # why? server does as leas
    ## NOTE: this doensn't work. If we do have multiple devices looking they'd be calling get() at different rates and displaying different results, adding load to the server. Define a timer function that calls get() at sepcified times (so you need to know the frequency of these actions), and then log those into a text file.
    global open_get
    return 
    pass
