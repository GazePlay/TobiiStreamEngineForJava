package tobii;

import java.awt.Dimension;
import java.awt.Toolkit;
import javax.swing.*;

public class TobiiDemo 
{
	public static class Wrapper<T>
	{
		public Wrapper(T obj) { this.obj = obj; }
		public T obj;
	}
	
	public static void main(String[] args) throws Exception
	{
		double screenWidth = Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		double screenHeight = Toolkit.getDefaultToolkit().getScreenSize().getHeight();
        
		Wrapper<JFrame> window = new Wrapper<>(new JFrame());
		
		SwingUtilities.invokeLater(new Runnable() 
		{
            public void run() 
            {
                window.obj.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        		JPanel panel = new JPanel();
        		panel.setPreferredSize(new Dimension(42, 100));
        		window.obj.add(panel);
        		window.obj.pack();
        		window.obj.setVisible(true);
            }
        });

		while (true)
		{
			float[] position = Tobii.gazePosition();
			position[0] *= screenWidth;
			position[1] *= screenHeight;
			//System.out.println(position[0] + " " + position[1]);
			window.obj.setLocation((int) position[0], (int) position[1]);
			Thread.sleep(16);
		}
	}
}
