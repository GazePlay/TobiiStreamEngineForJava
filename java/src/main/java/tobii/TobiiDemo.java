package tobii;

import java.awt.Toolkit;

public class TobiiDemo 
{
	public static void main(String[] args) throws Exception
	{
		double screenWidth = Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		double screenHeight = Toolkit.getDefaultToolkit().getScreenSize().getHeight();

		while (true)
		{
			float[] position = Tobii.gazePosition();
			System.out.print(position[0] + " " + position[1] + " - ");
			position[0] *= screenWidth;
			position[1] *= screenHeight;
			System.out.println(position[0] + " " + position[1]);
			Thread.sleep(1000);
		}
	}
}
