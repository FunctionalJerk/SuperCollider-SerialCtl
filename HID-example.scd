HID.findAvailable // initialize
HID.postAvailable // post all

~teensy = HID.open(5824, 1158); //open by vendorID & productID

(
var numPots = 4;
var numSwitches = 3;

~pots = numPots.collect{|index|
	{
		var msb = 0;
		var lsb = 0;
		var idArray = [0, 1] +.x [index*2];

		HIDFunc.element({ |...args|

			var combinedVal;
			var id = args.last % 2;
			var rawVal = args[1];

			if(id == 0) {
				msb = rawVal;
			} {
				lsb = rawVal;
			};

			combinedVal = lsb << 8 | msb;

			~buffer.set(index, combinedVal / 0xFFFF);

		}, idArray)
	}.value
};

~switches =	{
	var id = numPots * 2;
	var indices = (0..numSwitches-1);

	HIDFunc.element({ |...args|

		var rawVal = args[1];
		var values = rawVal >> indices & 0x01;

		~buffer.set(*lace([indices + numPots, values], numSwitches*2));

	}, id)

}.value;

)

(
Ndef(\test3, {

	var ctrl = PlayBuf.ar(7, ~buffer,loop: 1);
	var freqs = ctrl[..3].linexp(0.0,1.0,50,10000);
	var switches = ctrl[4..];
	switches.postln;
	Splay.ar(
		SinOscFB.ar(
			freq: freqs[..2],
			feedback: switches[0],
			mul: SinOscFB.ar(freqs[3], switches[1], switches[2], 1).range(0,1)
		)
	) * 0.1
}).play
)
