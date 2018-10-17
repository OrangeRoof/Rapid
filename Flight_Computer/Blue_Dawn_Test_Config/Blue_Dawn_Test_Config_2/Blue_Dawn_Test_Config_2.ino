///////////////////////////////////////////////////////////////////////////////////////////
// Bruin Spacecraft Group
// Software for interfacing with Rapid GSE
// Designed to output a constant stream of telemetry data, while responding to commands 
//
// Includes code from nff-sample.ino to process NanoRacks' nff data stream
// 
//     +----------------------------------------------------------+
//    /                   /                    /                / |
//   /                   /                    /                /  |
//  +---------------------------------------------------------+   |
//  |  ______                    _____                        |   |
//  |  | ___ \           ()     /  ___|                       |   |
//  |  | |_/ /_ __ _   _ _ _ __ \ `--. _ __   __  _  ___ ___  |   |
//  |  | ___ \ '__| | | | | '_ \ `--. \ '_ \ / _\| |/ __/ _ \ |   |
//  |  | |_/ / |  | |_| | | | | /\__/ / |_) | (_ | | (__| __/ |   +
//  |  \____/|_|   \__,_|_|_| |_\____/| .__/ \__/|_|\___\___| |  /
//  |                                 | |                     | /
//  |                                 |_|                     |/
//  +------------------------- * * * -------------------------+
//
///////////////////////////////////////////////////////////////////////////////////////////

//#include <FreqMeasure.h>       // For flowmeter, attaches to digital pin 8
#include <FreqCounter.h>       // alternative for flowmeter, attaches to digital pin 5
#include <SD.h>

//additionally required headers for sensor board
#include <Wire.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>
//NFF globals
#define NUMDATAFIELDS   21      // Number of data fields for each packet.
#define MAXBUFSIZE      201     // Maximum buffer size for serial packet (200)
#define MAXFIELDSIZE    20      // Maximum size of any data field in the serial packet.
#define SUCCESS         0       // Success return code.
#define ERROR           -1      // Error return code.

//Blue Dawn globals
#define MOSFET          4       // pin number for switch enable signal
#define FLOWMETER       5       // pin for freqCounter library, use 8 for freqMeasure

// Struct to contain all NFF flight data in one object.
struct NRdata
{
	char flight_state;
	double exptime;
	double altitude;
	double velocity[3];
	double acceleration[3];
	double attitude[3];
	double angular_velocity[3];
	bool liftoff_warn;
	bool rcs_warn;
	bool escape_warn;
	bool chute_warn;
	bool landing_warn;
	bool fault_warn;
} flight_info;


//bool mosfet_on = false;         // Used for checking if mosfet is on
int mosfet_on;
long int frequency = 0;           // Used for recording flow freq
long int timer = 999999;     // Used as coutndown timer to trigger switch

//function declarations
int parse_serial_packet(const char* buf, NRdata* flight_data);
String get_telemetry();
void write_to_register();
byte read_from_register();
void set_timer(int duration);
void set_pin(String type, int pinNumber);

void jump_reset();
bool sd_test();
// This calls the address 0 which apparently "resets" the arduino
void(* soft_reset) (void) = 0;

void setup()
{
	Serial.begin(115200);     // Set baud rate to 115200 (Default serial configureation is 8N1).
	Serial.setTimeout(20);    // Set timeout to 20ms (It may take up to 17ms for all of the data to
	// transfer from the NFF, this ensures that enough time has passed
	// to allow for a complete transfer before timing out).

	// Initialize the nff struct and all its data to 0.
	//memset(&flight_info, 0, sizeof(flight_info));
	//set pins
	pinMode(MOSFET, OUTPUT);

	//FreqMeasure.begin();
	// jump_reset Assumes there's a jump cable from pin D10 to RESET, don't use it for other things in this test!
	digitalWrite(10, HIGH);
	pinMode(10, OUTPUT);
}


void loop()
{
	//Serial.println("looping");
	//Init variables
	//String data_str;    //String for storing csv data, to be printed to serial
	//String nff_str;     //String for storing decoded nff data, 
	//optionally appended to data_str at the end of script
	//TODO: why MAXBUFSIZE twice? 
	char buffer[MAXBUFSIZE + 1 + MAXBUFSIZE] = {0};      // Buffer for receiving serial packets.
	int res = 0;                                // Value for storing results of function calls.

	// collect spacecraft time at start
	//double start_time = millis();
	//data_str = String(start_time); 
	// Read from serial up to the maximum serial packet size.
	res = Serial.readBytes(buffer, MAXBUFSIZE);
	// If no bytes are read, pass and continue collecting telemetry
	if (res != 0)
	{
		if (buffer[0] == 'c'){
			String temp_string = "";
			//TODO: rest of commands
			//GSE Command
			switch(buffer[1]){
				case 'p':
					Serial.println("GSE Ping Received");
					break;
				case 'm':
					if (buffer[2] == '1') {
						digitalWrite(MOSFET, HIGH);
					}
					else if (buffer[2] == '0') {
						digitalWrite(MOSFET, LOW);
					}
					break;
				case 't':
					for (int i = 2; i < res; i++){
						temp_string += buffer[i];
					}
					set_timer(temp_string.toInt());
					break;
				case 'r':
					// USING THIS FOR JUMP RESET
					Serial.println("GSE Reset Command Received");
					jump_reset();
					break;
				case 's':
					// USING THIS FOR SOFTWARE RESET
					Serial.println("GSE Soft Reset Received");
					soft_reset();
					break;
				case 'q':
          get_telemetry();
					break;
				case 'd':
					Serial.println("GSE Test SD Command Received");
					Serial.println(sd_test());
					break;
				default:
					Serial.println("error reading command: command not found");
			}
		}
	}
		//NFF Data
		/*else if (buffer[0] == 'n'){
			res = parse_serial_packet(buffer, &flight_info);
			nff_str += nr_str(&flight_info);
		}

		// Null terminate the buffer (Possibly unnecessary).
		buffer[res] = '\0'; 
	} 
	//Collect telemetry
	//data_str += get_telemetry();

	// Time elapsed
	double dt = millis() - start_time;
	data_str += "," + String(dt);

	if (nff_str != ""){
		data_str += nff_str;
	}*/

	//Serial.println("printing data str");
	//send data packet to GSE
	//Serial.println(data_str); 
	delay(1000);
}

bool sd_test() {
	// Sparkfun board uses pin 8 for CS control:
	// See Rapid Drive for J# configuration 
	SD.begin(8);
	File t = SD.open("test.txt");
	t.println("wholesome holes");
	t.close();
	File test = SD.open("test.txt");
	while(test.available()){
		Serial.write(test.read());
	}
	Serial.println();
	return(SD.remove("test.txt"));
}

void jump_reset() {
	// this function never returns. Assumes there's a jump cable from pin D10 to RESET.
	digitalWrite(10, LOW);
}

////////////
// get_telemetry:
// collect basic blue dawn telemetry and return csv string
////////////

String get_telemetry() {
	String telemetry = "";

	//Get Sensor Board Data
	//This assumes that 5V/GND are correctly connected,
	//SDA <-> A4 and SCL <-> A5
	//Just for a test
 
	Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(1000);
	lsm.begin();
	lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
	lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
	lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
/*
	float sensorData[] = {
		accl.acceleration.x,
		accl.acceleration.y,
		accl.acceleration.z,
		mag.magnetic.x,
		mag.magnetic.y,
		mag.magnetic.z,
		gyro.gyro.x,
		gyro.gyro.y,
		gyro.gyro.z
	};
	
	for(int x = 0; x < 9; x++){
		telemetry += "," + String(sensorData[x]);
	}*/
	// Mosfet
	mosfet_on = digitalRead(MOSFET);
	telemetry += "," + String(mosfet_on);
	//telemetry += "," + String(mosfet_on ? "1" : "0");

// arduino is low on memory
sensors_event_t a;
  lsm.getEvent(&a, NULL, NULL, NULL);

 
 Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(a.acceleration.y);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(a.acceleration.z);     Serial.println("  \tm/s^2");
  
lsm.getEvent(NULL, &a, NULL, NULL);
  // print out magnetometer data
  Serial.print("Magn. X: "); Serial.print(a.magnetic.x); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(a.magnetic.y);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(a.magnetic.z);     Serial.println("  \tgauss");

  lsm.getEvent(NULL, NULL, &a, NULL);
  // print out gyroscopic data
  Serial.print("Gyro  X: "); Serial.print(a.gyro.x); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(a.gyro.y);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(a.gyro.z);     Serial.println("  \tdps");


  
  return;

	// Flowmeter
	/*
	   double sum=0;
	   int count=0;
	   if (FreqMeasure.available()) {
	// average several reading together
	sum = sum + FreqMeasure.read();
	count = count + 1;
	if (count > 5) {
	telemetry += "," + String(FreqMeasure.countToFrequency(sum / count));
	sum = 0;
	count = 0;
	}
	}
	else {
	telemetry += ",0";
	}
	 */
	//TODO: test if this blocks or not
	FreqCounter::f_comp= 8;             // Set compensation to 12
	FreqCounter::start(1000);           // Start counting with gatetime of 100ms
	while (FreqCounter::f_ready == 0)   // wait until counter ready

		frequency = FreqCounter::f_freq;            // read result
	telemetry += "," + String(frequency);

	// timer
	long int time_remaining = timer - millis();
	telemetry += "," + String(time_remaining);
	if (time_remaining <= 0) {
		digitalWrite(MOSFET, HIGH);
	}


}


////////////
// nr_str:
// Return NR packet as a csv str
////////////

String nr_str(NRdata* flight_data) {

	String ret;
	ret += "," + String(flight_data->flight_state);
	ret += "," + String(flight_data->exptime);
	ret += "," + String(flight_data->altitude);

	for(int i = 0; i < 3; i++) {
		ret += "," + String(flight_data->velocity[i]);
	}

	for(int i = 0; i < 3; i++) {
		ret += "," + String(flight_data->acceleration[i]);
	}

	for(int i = 0; i < 3; i++) {
		ret += "," + String(flight_data->attitude[i]);
	}

	for(int i = 0; i < 3; i++) {
		ret += "," + String(flight_data->angular_velocity[i]);
	}

	ret += "," + String(flight_data->liftoff_warn ? "1" : "0");
	ret += "," + String(flight_data->rcs_warn ? "1" : "0");
	ret += "," + String(flight_data->escape_warn ? "1" : "0");
	ret += "," + String(flight_data->chute_warn ? "1" : "0");
	ret += "," + String(flight_data->landing_warn ? "1" : "0");
	ret += "," + String(flight_data->fault_warn ? "1" : "0");

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_serial_packet:
//
// Takes in a buffer containing an NFF serial data packet and parses the information into the 
// provided data struct.
//
// buf            =   Buffer containing serial data packet.
// flight_data    =   Struct containing current flight data.
//
// return   =   SUCCESS or ERROR
//
////////////////////////////////////////////////////////////////////////////////////////////////////
int parse_serial_packet(const char* buf, NRdata* flight_data)
{
	int res;                          // Value for storing result of function calls.
	int fieldnum = 1;                 // Index for field number being parsed.
	int index = 0;                    // Index for position in serial buffer.
	char temp[MAXFIELDSIZE] = { 0 };  // Temporary buffer for holding a data field.

	// If the buffer is empty then return with an error.
	if (strlen(buf) == 0)
	{
		return (ERROR);
	}

	// Continue parsing until the end of the buffer is reached
	while (index < strlen(buf))
	{
		// Scan the buffer from the current index until the next comma and place the data into the 
		// temporary buffer.
		res = sscanf((buf + index), "%[^,]", temp);

		// If sscanf failed to get a parameter then the buffer was not in the correct format so the 
		// function should return with an error.
		if (res == 0)
		{
			return (ERROR);
		}

		// Increment the buffer index by the length of the data field plus the comma.
		index += (strlen(temp) + 1);

		// If the index for the field number is greater than the expected number of data fields then 
		// the buffer is not correctly formatted so the function should return with an error.
		if (fieldnum > NUMDATAFIELDS)
		{
			return (ERROR);
		}

		// Depending upon the current data field being parsed, convert the data in the temporary buffer 
		// to the appropriate format and store it in the flight data struct.
		switch (fieldnum)
		{
			case 1:
				// Flight state is just a single character.
				flight_data->flight_state = temp[0];
				break;
			case 2:
				// atof() converts a c-string to a floating point number.
				flight_data->exptime = atof(temp);
				break;
			case 3:
				flight_data->altitude = atof(temp);
				break;
			case 4:
				flight_data->velocity[0] = atof(temp);
				break;
			case 5:
				flight_data->velocity[1] = atof(temp);
				break;
			case 6:
				flight_data->velocity[2] = atof(temp);
				break;
			case 7:
				flight_data->acceleration[0] = atof(temp);
				break;
			case 8:
				flight_data->acceleration[1] = atof(temp);
				break;
			case 9:
				flight_data->acceleration[2] = atof(temp);
				break;
			case 10:
				flight_data->attitude[0] = atof(temp);
				break;
			case 11:
				flight_data->attitude[1] = atof(temp);
				break;
			case 12:
				flight_data->attitude[2] = atof(temp);
				break;
			case 13:
				flight_data->angular_velocity[0] = atof(temp);
				break;
			case 14:
				flight_data->angular_velocity[1] = atof(temp);
				break;
			case 15:
				flight_data->angular_velocity[2] = atof(temp);
				break;
			case 16:
				// For the flight warnings, assign a boolean value of false for a 0 and true for anything 
				// else.
				flight_data->liftoff_warn = (temp[0] == '0') ? false : true;
				break;
			case 17:
				flight_data->rcs_warn = (temp[0] == '0') ? false : true;
				break;
			case 18:
				flight_data->escape_warn = (temp[0] == '0') ? false : true;
				break;
			case 19:
				flight_data->chute_warn = (temp[0] == '0') ? false : true;
				break;
			case 20:
				flight_data->landing_warn = (temp[0] == '0') ? false : true;
				break;
			case 21:
				flight_data->fault_warn = (temp[0] == '0') ? false : true;
				break;
		}

		// Increment the index for the current data field.
		fieldnum++;
	}

	// If the correct number of fields have been parsed then return with succcess, otherwise return 
	// with an error.
	if (fieldnum == (NUMDATAFIELDS + 1))
	{
		return (SUCCESS);
	}
	else
	{
		return (ERROR);
	}
}

//GSE Command Functions

void write_to_register(){
	;
}
byte read_from_register(){
	;
}
void set_timer(int duration) {
	;
}


