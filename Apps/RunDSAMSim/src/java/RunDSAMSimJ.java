import java.util.HashMap;

public class RunDSAMSimJ {
	private static double dt = 0.0;
	private static boolean	staticTimeFlag = false;
	private static short	numChannels = 0;
	private static short	numWindowFrames = 0;
	private static double	outputTimeOffset = 0.0;
	private static short	interleaveLevel = 1;
	private static long	length = 0;
	private static double[] labels = null;

	/**
	 * @param args
	 */
	private static native double[][] RunSimScriptCPP(String simFile,
	  String pars, double inChannels[][]);
	private static void PrintOutput(double channels[][]) {
		if (channels == null)
			return;
		for (int chan = 0; chan < numChannels; chan++)
			System.out.printf("\t" + labels[chan]);
		System.out.println();
		for (int i = 0; i < length; i++) {
			System.out.printf(i + ": ");
			for (int chan = 0; chan < numChannels; chan++)
				System.out.printf("\t" + channels[chan][i]);
			System.out.println();
		}
	}
	private static HashMap<String, Object> CreateTestHMap() {
		HashMap<String, Object> hMap = new HashMap<String, Object>();
		hMap.put("dt", 0.01e-3);
		hMap.put("numChannels", (short) 2);
		hMap.put("length", (long) 100);
		return hMap;
	}
	private static void SetInfoFields(HashMap<String, Object> hMap) {
		if (hMap.containsKey("dt")) {
			dt = (Double) hMap.get("dt");
		}
		if (hMap.containsKey("staticTimeFlag")) {
			staticTimeFlag = (Boolean) hMap.get("staticTimeFlag");
		}
		if (hMap.containsKey("numChannels")) {
			numChannels = (Short) hMap.get("numChannels");
		}
		if (hMap.containsKey("numWindowFrames")) {
			numWindowFrames = (Short) hMap.get("numWindowFrames");
		}
		if (hMap.containsKey("outputTimeOffset")) {
			outputTimeOffset = (Double) hMap.get("outputTimeOffset");
		}
		if (hMap.containsKey("interleaveLevel")) {
			interleaveLevel = (Short) hMap.get("interleaveLevel");
		}
		if (hMap.containsKey("length")) {
			length = (Long) hMap.get("length");
		}
	}
	public static double	GetDt()	{ return dt; }
	public static boolean	GetStaticTimeFlag()	{ return staticTimeFlag; }
	public static short		GetNumChannels()	{ return numChannels; }
	public static short		GetNumWindowFrames()	{ return numWindowFrames; }
	public static double	GetOutputTimeOffset()	{ return outputTimeOffset; }
	public static short		GetInterleaveLevel()	{ return interleaveLevel; }
	public static long		GetLength()		{ return length; }
	public static double[]	GetLabels()		{ return labels; }
	public static boolean SetDt(double theDt) {
		dt = theDt;
		return true;
	}
	public static boolean SetStaticTimeFlag(boolean theStaticTimeFlag) {
		staticTimeFlag = theStaticTimeFlag;
		return true;
	}
	public static boolean SetNumWindowFrames(short theNumWindowFrames) {
		numWindowFrames = theNumWindowFrames;
		return true;
	}
	public static boolean SetOutputTimeOffset(double theOutputTimeOffset) {
		outputTimeOffset = theOutputTimeOffset;
		return true;
	}
	public static boolean SetInterleaveLevel(short theInterleaveLevel) {
		interleaveLevel = theInterleaveLevel;
		return true;
	}
	public static void PrintFields() {
		System.out.println("dt = " + dt);
		System.out.println("staticTimeFlag = " + staticTimeFlag);
		System.out.println("numChannels = " + numChannels);
		System.out.println("numWindowFrames = " + numWindowFrames);
		System.out.println("outputTimeOffset = " + outputTimeOffset);
		System.out.println("interleaveLevel = " + interleaveLevel);
		System.out.println("length = " + length);
	}
	public static double[][] RunSimScript(String simFile, String parOptions,
			double inSignal[][], HashMap<String, Object> hMap) {
		SetInfoFields(hMap);
		return(RunSimScriptCPP(simFile, parOptions, inSignal));
	}
	public static double[][] RunSimScript(String simFile, String parOptions,
			double inSignal[][]) {
		return(RunSimScriptCPP(simFile, parOptions, inSignal));
	}
	public static double[][] RunSimScript(String simFile, String parOptions) {
		return(RunSimScriptCPP(simFile, parOptions, null));
	}
	public static double[][] RunSimScript(String simFile) {
		return(RunSimScriptCPP(simFile, "", null));
	}
	public static void main(String[] args) {
		System.out.println("Hello Java World");
		double inSignal[][] = null;
		double freq = 500.0;

		HashMap<String, Object> hMap = CreateTestHMap();
		SetInfoFields(hMap);
		PrintFields();
		if (length > 0) {
			inSignal = new double[numChannels][(int) length];
			for (int chan = 0; chan < numChannels; chan++)
				for (int i = 0; i < length; i++) {
					inSignal[chan][i] = Math.sin(2.0 * Math.PI * freq * i * dt +
					  Math.PI / (chan + 1));
				}
		}
		//double[][] channels = RunSimScript("test.spf", "dt 0.00001", null);
		//double[][] channels = RunSimScript("test2.spf", "", inSignal);
		double[][] channels = RunSimScript(args[0], "", inSignal, hMap);
		//PrintOutput(channels);
		PrintFields();
		inSignal = null;
	}
	static {
		System.loadLibrary("LibRunDSAMSim");
	}

}
