# Blue Dawn Telemetry Processor:
# The Blue Dawn flight computer returns something. Not quite sure what it is yet, just processing code as it comes in

x = 0
def process(bytes):
    global x
    # make everything floats, zip em up and return the list of tuples
    x+=1
    return([("test",x)])
