# Blue Dawn Telemetry Processor:
# The Blue Dawn flight computer returns something. Not quite sure what it is yet, just processing code as it comes in
__packetmap = ['spacecraft_time','bd_acceleration_x','bd_acceleration_y','bd_acceleration_z','bd_ang_vel_x', 'bd_ang_vel_y', 'bd_ang_vel_z','bd_mag_x','bd_mag_y','bd_mag_z', 'mosfet_state', 'flow_rate','timer','dt','voltage','current', 'flight_state', 'exp_time', 'altitude', 'velocity_x', 'velocity_y', 'velocity_z','attitude_x','attitude_y','attitude_z','acceleration_x','acceleration_y','acceleration_z','ang_vel_x','ang_vel_y','ang_vel_z','liftoff_warning','rcs_warning','escape_warning','chute_warning','landing_warning','fault_warning']

def process(bytes):
    # the bytes are actually UTF-8 in csv, so lets make that so
    d = bytes.decode('utf-8').split(',')

    # and bring in everything from the old code
    if len(d) > 17:
        for i in range(2):
            tmp = d.pop()
            d.insert(14, tmp)

    # convert flowmeter freq to flow rate
    if d[11] != "0":
        d[11] = float(d[11]) * 2.4/880 - 0.0009
        d[11] = str(d[11])

    # make everything floats, zip em up and return the list of tuples
    return(list(zip(__packetmap, [float(x) for x in d])))
