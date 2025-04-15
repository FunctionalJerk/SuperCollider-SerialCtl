
This example is tuned very specifically to the serial device described in:
https://github.com/FunctionalJerk/SuperCollider-SerialCtl

(

// execute once:
//
// for me, serial-devices always appeared as "/dev/ttyACM0" or "/dev/ttyACM1".
// In either case, they always were at index 0 of SerialPort.devices.
// if this doesn't work for you, go:
// SerialPort.listDevices
// and copy and paste the string pointing to your device in SerialPort's port argument below
var port = SerialPort.devices[0];

~controls = Bus.control(s, 7);

p = SerialPort(
	port: port,
	baudrate: 230400,    //check that baudrate is the same as in the sketch
	exclusive: true,
	xonxoff: true,
);
p.doneAction = { ("serial port \"" ++ port ++ "\" disconnected").postln; };

)

(
// 14bit resolution analogread
// struct size of 8 bytes
var deconstruct;
var keys = [\pot1, \pot2, \pot3, \pot4, \sw1, \sw2, \sw3];

~struct = ();

deconstruct = {|buffer|

	var values = [
		((buffer[1] & 0x3F ) << 8) | buffer[0],                          // pot 1
		(buffer[3] & 0xF << 10) | (buffer[2] << 2) | (buffer[1] >> 6),   // pot 2
		((buffer[5] & 0x3) << 12) | (buffer[4] << 4) | ((buffer[3] >> 4) & 0xF), // pot 3
		(buffer[5] >> 2) | (buffer[6] << 6),                             // pot 4

		(buffer[7] >> 0 & 0x1), // switch 1
		(buffer[7] >> 1 & 0x1), // switch 1
		(buffer[7] >> 2 & 0x1)  // switch 1
	];

	values
};

r = Routine.run({

	var numbytes = 8;

	inf.do {
		var buffer = numbytes.collect{ p.read };

		buffer = deconstruct.(buffer);
		~controls.setnSynchronous(buffer);        // to control rate bus
		~struct.putAll(lace([keys, buffer], 14)); // to dictionary/event
	}
});
)

(
l = r { inf.do {
	"=======================".postln;
	~struct.postln;
	1.wait;
} }.play

)

//// Uncomment to monitor the control-busses:
// fork { loop { ~controls.getnSynchronous.postln; 0.5.wait } };
//// If you want to stop the Routine and SerialPort with CmdPeriod, uncomment:
// CmdPeriod.doOnce{ r.stop; p.close };


(
// very basic sound example:
// you may hear steps, it
SynthDef(\test, {|out|

	Out.ar(out,
		VarSaw.ar(~controls.kr(2).linexp(1,16383,100,16483).poll,0,~controls.kr(2,2) / 16383,0.3 * ~controls.kr(3,4))
	)
}).play(s)
)
