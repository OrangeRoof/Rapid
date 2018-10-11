# High Level Description of GSE Telemetry Processor
## Overview
  The Bruinspace GSE allows different missions to write custom telemetry processors to suit their needs. Here's what that means:
  ```
  serial_manager -> [mission-specifc TP] -> GUI
  (read bytes)   -> (do stuff to bytes)  -> (show data)
  ```
  Our GSE lets you open and read from multiple serial devices at once, returning raw bytes:
  ```py
  import serial_manager
  serial_manager.open(['COM1', 'COM13'])
  
  # returns [(bytes1, 'COM1'), (bytes2, 'COM13']
  # (assuming both ports were opened correctly)
  serial_manager.read()
  ```
  So you could do something like this:
  ```py
  import blue_dawn_tp, umbilical_tp
  for m, p in serial_manager.read():
    # different serial ports get processed differently!
    if p == blue_dawn_tp.port:
        blue_dawn_tp.process(m)
    elif p == umbilical_tp.port:
        umbilical_tp.process(m)
 ```
 Where `tp.process` converts raw data into something that the GUI can read!

## Specifications
 The GSE expects:
 - in the `tp` folder, a python file `your_tp.py` (the telemetry processor)
 - `your_tp.py` must implement `process(raw_bytes)`:
        - input: a python `bytes` object
        - output: a list of tuples `(name, data)`, specifying the titles and values of the data to display
        - `name` must be of type `str`; `data` must be castable to `float`
        - *e.g. `[('flight_x', 100.3), ('flight_y', 200.135)]`*
 - `your_tp.py` can have anything else you want: globals, other functions, other imports
        - I mean, within limits. Remember `process(raw_bytes)` is meant to be called at a very high frequency. It's kinda hard to do that with `os.system('curl parrot.live')` and blocking execution

## Usage
 After creating `tp/your_tp.py`, check on the web console. You'll have the option to link `your_tp` to a serial port. Doing so tells the serial monitor to open that serial port, and pipe bytes into `your_tp.process(raw_bytes)`, and display the returned list on the web portal.
