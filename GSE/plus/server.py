from flask import Flask, render_template, request
import tp_manager
import serial_manager_plus

app = Flask(__name__)

@app.route('/')
@app.route('/config')
def genconfig():
    tp_manager.import_tp()
    return render_template('config.j2', sptp = (serial_manager_plus.get_ports(), tp_manager.get_tp_name()))

@app.route('/makeconf', methods=['GET'])
def setconfig():
    p = list(request.args.items())
    # n[1] is the tp, remove not selected items ('' evaluates to false)
    p = list(filter(lambda n: n[1], p))
    # open serial ports, factory function
    return(str(s))

if __name__ == '__main__':
    app.run(debug=True)
