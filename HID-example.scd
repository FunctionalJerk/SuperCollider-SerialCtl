HID.findAvailable // initialize
HID.postAvailable // post all

~teensy = HID.open(5824, 1158); //open by vendorID & productID

HIDFunc

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
Ndef(\test, {

	var ctrl = PlayBuf.ar(7, ~buffer,loop: 1);
	var freqs = ctrl[..1].lag(0.02).linexp(0.0,1.0,10,10000);
	var muls = ctrl[2..3];
	var switches = ctrl[4..];

	Splay.ar(
		SinOscFB.ar(
			freq: freqs,
			feedback: SinOsc.ar(freqs.rotate / (switches[1] * pi).lag(0.5), 0, muls[0] * pi, 1),
		) * SelectX.kr(switches[0].lag(1), [
			SinOscFB.ar(freqs.rotate * (muls[0] + 0.5),  muls[1] * pi, switches[2].lag(0.5) * 2, 1),
			LFNoise0.kr(freqs.rotate * (muls[1] * pi + pi), switches[2].lag(0.5) * 2, 1)
		])
	) * 0.1
}).play
)