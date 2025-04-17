
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

s.waitForBoot {
	~controlBus = Bus.control(s, 7);
	~buffer = Buffer.alloc(s, 1, 7);
};

p = SerialPort(
	port: port,
	// databits: 14,
	baudrate: 115200,
	databits: 8,       // Explicitly set data bits (default is 8)
	stopbit: false,    // 1 stop bit (false = 1, true = 2)
	parity: nil,       // No parity
	xonxoff: false,    // Disable software flow control
	crtscts: false,    // Disable hardware flow control (explained below)
	exclusive: true
);
p.doneAction = { ("serial port \"" ++ port ++ "\" disconnected").postln; };

)

(
// 14bit resolution analogread
// struct size of 8 bytes
var reconstruct;
var startMarker = 0xAA;
var endMarker = 0x55;
var controls;
var numbytes = 8;
// var keys = [\pot1, \pot2, \pot3, \pot4, \sw1, \sw2, \sw3];

// ~struct = ();

reconstruct = {|array|

	var values = [
		((array[1] & 0x3F ) << 8) | array[0],                          // pot 1
		(array[3] & 0xF << 10) | (array[2] << 2) | (array[1] >> 6),    // pot 2
		((array[5] & 0x3) << 12) | (array[4] << 4) | ((array[3] >> 4) & 0xF), // pot 3
		(array[5] >> 2) | (array[6] << 6),                             // pot 4

		(array[7] & 0x1), // switch 1
		(array[7] >> 1 & 0x1), // switch 1
		(array[7] >> 2 & 0x1)  // switch 1
	];

	values
};

r = Routine.run({
	var byte, lastByte, checksum;
	var expectedChecksum = numbytes;
	var normedControls;

	inf.do {
		lastByte = byte; byte = p.read;
		checksum = 0;

		if( byte == startMarker and: (lastByte == endMarker) ) {

			controls = numbytes.collect { p.read };

			expectedChecksum = p.read;

			controls.do {|b| checksum = checksum bitXor: b };

			if( expectedChecksum == checksum ) {
				controls = reconstruct.(controls);
				// controls.postln;
				normedControls = controls[..3] / 0x3FFF ++ controls[4..];
				~buffer.set(*lace([(0..6), normedControls], 14));
				// ~controls = controls;                       // look at them
				// ~controlBus.setnSynchronous(normedControls);      // write to control rate bus
				// ~struct.putAll(lace([keys, controls], 14)); // store in dictionary/event

			} { "checksum doesn't check out: % vs. %\n".postf(expectedChecksum, checksum) }
		} {  };
	}
});
)
r.stop

{ ~controlBus.kr(4) }.scope

//// monitor the control-busses:
r { inf.do { ~controlBus.getnSynchronous.postln; 0.5.wait } }.play;
//// monitor the Buffer:
r { inf.do { ~buffer.getn(0, ~buffer.numFrames * ~buffer.numChannels, {|f| f.postln }); 0.5.wait; } }.play;
//// monitor the 'dictionary' (it's an Event, but OK):
r { inf.do { ~struct.postln; 0.5.wait; } }.play;

//// If you want to stop the Routine and SerialPort with CmdPeriod, execute:
CmdPeriod.doOnce{ r.stop; p.close };

/////////////////////////////////////////////
// very basic sound examples
// you may hear steps, it depends on your ControlRate (s.options.sampleRate / s.options.blockSize)
(
// BUS
Ndef(\x, {

	VarSaw.ar(
		freq: ~controlBus.kr(2).linexp(1,16383,100,16483).poll,
		iphase: 0,
		width: ~controlBus.kr(2,2) / 16383,
		mul: 	0.3 * ~controlBus.kr(3,4)
	)
}).play
)
1 / 750
(
// BUFFER
Ndef(\x, {

	var phase = Phasor.ar(rate: BufRateScale.kr(~buffer), start: 0, end: 1.0);
	var buf = VarLag.ar(BufRd.ar(7, ~buffer.bufnum, phase: phase, loop: 1), 0.0075, warp: \sine);
	// var freq = BufRd.ar(7, ~buffer.bufnum, phase: phase, loop: 1);
	// var trig = Changed.kr(buf[4..]);

	var freq = buf[..3].linexp(0,1,50,14000);
	// var freq = [MouseX.kr(50,14000,1, 0.0), MouseY.kr(50,14000,1,0.0)];

	// var freq = Phasor.kr(0, 1, 0, 300).linexp(0,300,50,14000);

	// freq.poll;
	Out.ar(0,Splay.ar(SinOsc.ar(freq)) * 0.1)

}).play
)

