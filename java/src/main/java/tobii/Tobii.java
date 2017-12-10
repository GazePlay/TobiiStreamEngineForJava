package tobii;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class Tobii
{
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
		loadLibraryFromResources("/lib/tobii/x64/tobii_stream_engine.dll");
		loadLibraryFromResources("/lib/tobii/x64/tobii_jni_stream_engine.dll");
		int code = jniInit();
		if (code != 0)
		{
			System.out.println("Error during init");
		}
	}

	public static void loadLibraryFromResources(String filePath)
	{
		try
		{
			InputStream in = Tobii.class.getResourceAsStream(filePath);
			byte[] buffer = new byte[1024];
			int read = -1;
			String tmpDir = System.getProperty("java.io.tmpdir");
			File tmpFile = new File(tmpDir, filePath);
			tmpFile.getParentFile().mkdirs();
			FileOutputStream fos = new FileOutputStream(tmpFile);
			while((read = in.read(buffer)) != -1) 
			{
				fos.write(buffer, 0, read);
			}
			fos.close();
			in.close();
			System.load(tmpFile.getAbsolutePath());
		}
		catch (IOException e)
		{
			throw new RuntimeException(e);
		}
	}

	private native int jniInit();

	private native float[] jniGazePosition();
}
