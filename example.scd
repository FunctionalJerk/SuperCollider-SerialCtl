
(
// for me, serial-devices always appeared as "/dev/ttyACM0" or "/dev/ttyACM1"
// always at index 0
// if this doesn't wor
var port = SerialPort.devices[0];

p = SerialPort(
	port,    // SerialPort.listDevices
    baudrate: 9600,    //check that baudrate is the same as in the sketch
	exclusive: true,
    xonxoff: true,
);
p.doneAction = { ("serial port \"" ++ port ++ "\" disconnected").postln; };

)

(
~pots = Bus.control(s, 7);

r = Routine({
    var byte, str;
	var which = 0;
	var val = 0;

    loop {
        if(p.read==10, {
            str = "";

            while({byte = p.read; byte != 13 }, {
                str = str ++ byte.asAscii
            });

			# which, val = str.split($;).asInteger;
			~pots.setAt(which, val);
        })
    }
}).play;

// monitor the control-busses
fork { loop { ~pots.getnSynchronous.postln; 0.5.wait } };
)

(
// this is just a very basic example on how to use the serial input.
SynthDef(\test, {|out|

	Out.ar(out,
		SinOsc.ar(~pots.kr(2).linexp(0,1023,100,4000),0,0.3)
	)
}).play
)