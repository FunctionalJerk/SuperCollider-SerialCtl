
// For checking


HID.findAvailable // initialize
HID.postAvailable // post all

~teensy = HID.open(5824, 1158, "/dev/hidraw7"); //open by vendorID & productID
// ~trackpad = HID.open(9094, 17207);

// Check to see whats coming in:
HIDFunc.trace;

(
// set up for both scenarios
~hidBus = Bus.control(s, 7);
~buffer = Buffer.alloc(s,1,7);

//
~testBuf = Buffer.alloc(s, 1,1);
~testBus = Bus.control(s,1);
// 1 sec test-buffer for plotting
~plot = Buffer.alloc(s, s.sampleRate, 1);

)

// check to see what's in the buffer / bus
~buffer.loadToFloatArray(0,-1, _.postln)
~hidBus.getn(7, _.postln);
{ ~hidBus.kr }.scope;


////////////////////////////////
// HIDFunc

(
var numPots = 4;
var numSwitches = 3;

// pots
numPots.collect{|index|
	{
		var msb = 0;
		var lsb = 0;
		var idArray = [0, 1] + (index*2);

		HIDFunc.element({ |...args|

			var combinedVal;
			var id = args.last % 2;
			var rawVal = args[1];

			if(id == 0) {
				lsb = rawVal;
				combinedVal = msb << 8 | lsb;
				// ~buffer.set(index, combinedVal / 0xFFFF);
				~hidBus.setAt(index, [combinedVal / 0xFFFF]);
			} {
				msb = rawVal;
				combinedVal = msb << 8 | lsb;
			};

		}, idArray).permanent_(false)
	}.value
};

// switches
{
	var id = numPots * 2;
	var indices = (0..numSwitches-1);

	HIDFunc.element({ |...args|

		var rawVal = args[1];
		var values = rawVal >> indices & 0x01;

		// ~buffer.set(*lace([indices + numPots, values], numSwitches*2));
		~hidBus.setnAt(4, values);

	}, id).permanent_(false)

}.value;
)

(
Ndef(\test, {|freq=0|

	var ctrl = In.kr(~hidBus, 7);
	// var ctrl = PlayBuf.ar(7, ~buffer,loop: 1);
	var freqs = ctrl[..1].lag(0.0).linexp(0.0,1.0,10,8000).lag(0.05);
	var muls = ctrl[2..3];
	var switches = ctrl[4..];
	// ctrl.poll;

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
