package tobii;

import java.io.File;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

public class Tobii
{
    public static boolean verbose = true;

    public static String dataDirectoryPath = System.getenv("LocalAppData") + "/TobiiStreamEngineForJava";

    private static boolean loaded = false;

    private static boolean errorReported = false;

    public static float[] gazePosition()
    {
        try
        {
            loadIfNotLoaded();
            return jniGazePosition();
        }
        catch (Throwable e)
        {
            if (!errorReported)
            {
                e.printStackTrace();
                errorReported = true;
            }
            return new float[] { 0.5f, 0.5f };
        }
    }

    private static void loadIfNotLoaded() throws Exception
    {
        if (loaded) return;
        loaded = true;
        loadNeededLibraries();
        int code = jniInit();
        printIfVerbose("Init code error " + code);
    }

    private static void loadNeededLibraries() throws Exception
    {
        printIfVerbose("Loading needed libraries using directory " + dataDirectoryPath);

        copyResourceIntoDir("/lib/tobii/x64/vcruntime140.dll", dataDirectoryPath);
        copyResourceIntoDir("/lib/tobii/x64/msvcp140.dll", dataDirectoryPath);
        copyResourceIntoDir("/lib/tobii/x64/concrt140.dll", dataDirectoryPath);
        copyResourceIntoDir("/lib/tobii/x64/vccorlib140.dll", dataDirectoryPath);
        copyResourceIntoDir("/lib/tobii/x64/tobii_stream_engine.dll", dataDirectoryPath);
        copyResourceIntoDir("/lib/tobii/x64/tobii_jni_stream_engine.dll", dataDirectoryPath);

        // loadLibrary(tmpDir, "/lib/tobii/x64/vcruntime140.dll");
        // loadLibrary(tmpDir, "/lib/tobii/x64/msvcp140.dll");
        // loadLibrary(tmpDir, "/lib/tobii/x64/concrt140.dll");
        // loadLibrary(tmpDir, "/lib/tobii/x64/vccorlib140.dll");
        // loadLibrary(tmpDir, "/lib/tobii/x64/tobii_stream_engine.dll");
        loadLibrary(dataDirectoryPath, "/lib/tobii/x64/tobii_jni_stream_engine.dll");
    }

    private static void copyResourceIntoDir(String resourceFilePath, String dirPath) throws Exception
    {
        printIfVerbose("Copying " + resourceFilePath + " into " + dirPath);
        InputStream in = Tobii.class.getResourceAsStream(resourceFilePath);
        File tmpFile = new File(dirPath, resourceFilePath);
        tmpFile.getParentFile().mkdirs();
        Files.copy(in, tmpFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
        in.close();
    }

    private static void loadLibrary(String dirPath, String filePath)
    {
        File tmpFile = new File(dirPath, filePath);
        printIfVerbose("Loading library " + tmpFile.getAbsolutePath());
        System.load(tmpFile.getAbsolutePath());
    }

    private static void printIfVerbose(String what)
    {
        if (verbose)
        {
            System.out.println("Tobii: " + what);
        }
    }

    private static native int jniInit();

    private static native float[] jniGazePosition();
}
