from flask import Flask, render_template, request, redirect, url_for
import tp_manager
import serial_manager_plus
import flask_helper

app = Flask(__name__)

@app.route('/')
@app.route('/config')
def genconfig():
    tp_manager.import_tp()
    return render_template('config.j2', sptp = (serial_manager_plus.get_ports(), tp_manager.get_tp_name()))

@app.route('/makeconf', methods=['GET'])
def setconfig():
    p = list(request.args.items())
    flask_helper.start_serial(p)
    return(redirect(url_for('monitor')))

@app.route('/get', methods=['GET'])
def getport():
    p = list(request.args.items())
    ## [0][0]: each item is a 2-tuple (key, value): we just need the key
    return(flask_helper.read(p[0][0]))

@app.route('/monitor')
def monitor():
    # serve monitor page here
    pass

if __name__ == '__main__':
    app.run(debug=True)
