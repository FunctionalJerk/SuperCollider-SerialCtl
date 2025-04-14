
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

~switches = Bus.control(s,4);
~pots = Bus.control(s,4);

p = SerialPort(
	port: port,
    baudrate: 115200,    //check that baudrate is the same as in the sketch
	exclusive: true,
    xonxoff: true,
);
p.doneAction = { ("serial port \"" ++ port ++ "\" disconnected").postln; };

)

(

r = Routine.run({
	// least & most significant byte
	var lsb = 0;
	var msb = 0;

	var index = 0;
	var item = 0;

	loop {
		msb = p.read; lsb = p.read;

		if((msb == 255) && (lsb == 255)) { index = 0 } {
			// see: https://github.com/FunctionalJerk/SuperCollider-SerialCtl/blob/main/src/main.cpp#L62
			if(index == 0) {
				item = lsb.asBinaryDigits.reverse[..3]; // only the last four bits are used here
				~switches.setnSynchronous(item);
			} {
				item = (msb << 8) | lsb;
				~pots.setAt(index - 1, item);
			};
			index = index + 1 % 5;
		};
	}
});

//// Uncomment to monitor the control-busses:
// fork { loop { ~pots.getnSynchronous.postln; 0.5.wait } };
//// If you want to stop the Routine with CmdPeriod, uncomment:
// CmdPeriod.doOnce{ r.stop; };
)

(
// This is just a very basic example on how to use the serial input:
// It uses the first two of the four pots to control the frequency,
// and the other two to control the width of a VarSaw Ugen.
// They can be silenced by the first two switches.
SynthDef(\test, {|out|

	Out.ar(out,
		VarSaw.ar(~pots.kr(2).linexp(1,16383,100,800),0,~pots.kr(2,2) / 16383,0.3 * ~switches.kr(2))
	)
}).play(s)
)
