import os
import importlib
telemetry_processors = []

def import_tp():
    global telemetry_processors
    # assumes tp folder is in the same directory as current. Looks for all
    # valid tp.py files (i.e. has process()) in /tp
    # then returns it as  ziplist [('name', tp_obj)]
    pyf = list(filter(lambda f: f.endswith('.py'), os.listdir('./tp')))
    for tp in pyf:
        # so: import_module does that, but takes the name of the module as input;
        # its also in the tp folder, which we can have the function treat as a package
        # 'anything.py' has last three characters as filetype, hence use everything up tho the last three
        # (tp[:-3])
        tpm = importlib.import_module('tp.'+tp[:-3])
        # there's no way to actually check if process returns the correct type without
        # passing it the right data, which beats the point
        if 'process' in dir(tpm):
            telemetry_processors.append((tp[:-3], tpm))

def get_tp_name():
    global telemetry_processors
    # take every ('name', tp_obj) tuple in the tp list, and take 'name'
    return(list(map(lambda t: t[0], telemetry_processors)))

def get_tp(name):
    global telemetry_processors
    return(list(filter(lambda x: x[0] == name for x in telemetry_processors))[0][1])
