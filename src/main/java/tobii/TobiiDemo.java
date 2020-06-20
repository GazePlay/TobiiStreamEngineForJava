package tobii;

import java.awt.*;

public class TobiiDemo {
    public static void main(String[] args) throws Exception {

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        int screenWidth = gd.getDisplayMode().getWidth();
        int screenHeight = gd.getDisplayMode().getHeight();

        System.out.println("screenWidth = " + screenWidth + ", screenHeight = " + screenHeight);



        while (true) {
            float[] position = Tobii.gazePosition();

            float xRatio = position[0];
            float yRatio = position[1];

            int xPosition = (int) (xRatio * screenWidth);
            int yPosition = (int) (yRatio * screenHeight);

            String message = "xRatio = " + xRatio +
                    ", " +
                    "yRatio = " + yRatio +
                    ", " +
                    "xPosition = " + xPosition +
                    ", " +
                    "yPosition = " + yPosition;
            System.out.println(message);

            Thread.sleep(1000);
        }
    }
}
