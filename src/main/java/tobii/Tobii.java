package tobii;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class Tobii
{
	public static boolean verbose = true;
	
	public static float[] gazePosition()
	{
		if (instance == null)
		{
			instance = new Tobii();
		}
		return instance.jniGazePosition();
	}

	private static Tobii instance = null;

	private Tobii()
	{
		loadNeededLibraries();
		int code = jniInit();
		printIfVerbose("Init code error " + code);
	}
	
	private static void loadNeededLibraries()
	{
		printIfVerbose("Loading needed libraries");
		String tmpDir = System.getProperty("java.io.tmpdir");
		printIfVerbose("Using tmpdir " + tmpDir);
		
		copyResourceIntoDir("/lib/tobii/x64/vcruntime140.dll", tmpDir);
		copyResourceIntoDir("/lib/tobii/x64/msvcp140.dll", tmpDir);
		copyResourceIntoDir("/lib/tobii/x64/concrt140.dll", tmpDir);
		copyResourceIntoDir("/lib/tobii/x64/vccorlib140.dll", tmpDir);
		copyResourceIntoDir("/lib/tobii/x64/tobii_stream_engine.dll", tmpDir);
		copyResourceIntoDir("/lib/tobii/x64/tobii_jni_stream_engine.dll", tmpDir);
		
		loadLibrary(tmpDir, "/lib/tobii/x64/vcruntime140.dll");
		loadLibrary(tmpDir, "/lib/tobii/x64/msvcp140.dll");
		loadLibrary(tmpDir, "/lib/tobii/x64/concrt140.dll");
		loadLibrary(tmpDir, "/lib/tobii/x64/vccorlib140.dll");
		loadLibrary(tmpDir, "/lib/tobii/x64/tobii_stream_engine.dll");
		loadLibrary(tmpDir, "/lib/tobii/x64/tobii_jni_stream_engine.dll");
	}
	
	private static void copyResourceIntoDir(String resourceFilePath, String dirPath)
	{
		try
		{
			printIfVerbose("Copying " + resourceFilePath + " into " + dirPath);
			InputStream in = Tobii.class.getResourceAsStream(resourceFilePath);
			byte[] buffer = new byte[1024];
			int read = -1;
			File tmpFile = new File(dirPath, resourceFilePath);
			tmpFile.getParentFile().mkdirs();
			FileOutputStream fos = new FileOutputStream(tmpFile);
			while((read = in.read(buffer)) != -1) 
			{
				fos.write(buffer, 0, read);
			}
			fos.close();
			in.close();
		}
		catch (Exception e)
		{
			throw new RuntimeException(e);
		}
	}

	private static void loadLibrary(String dirPath, String filePath)
	{
		try
		{
			File tmpFile = new File(dirPath, filePath);
			printIfVerbose("Loading library " + tmpFile.getAbsolutePath());
			System.load(tmpFile.getAbsolutePath());
		}
		catch (Exception e)
		{
			throw new RuntimeException(e);
		}
	}
	
	private static void printIfVerbose(String what)
	{
		if (verbose)
		{
			System.out.println("Tobii: " + what);
		}
	}

	private native int jniInit();

	private native float[] jniGazePosition();
}
