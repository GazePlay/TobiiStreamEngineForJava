package tobii;

import java.awt.*;

public class TobiiDemo {
    public static void main(String[] args) throws Exception {

        Toolkit defaultToolkit = Toolkit.getDefaultToolkit();
        Dimension screenSize = defaultToolkit.getScreenSize();
        double screenWidth = screenSize.getWidth();
        double screenHeight = screenSize.getHeight();
        System.out.println("screenWidth = " + screenWidth + ", screenHeight = " + screenHeight);

        while (true) {
            float[] position = Tobii.gazePosition();

            float xRatio = position[0];
            float yRatio = position[1];

            int xPosition = (int) (xRatio * screenWidth);
            int yPosition = (int) (yRatio * screenHeight);

            StringBuilder message = new StringBuilder();
            message.append("xRatio = ").append(xRatio);
            message.append(", ");
            message.append("yRatio = ").append(yRatio);
            message.append(", ");
            message.append("xPosition = ").append(xPosition);
            message.append(", ");
            message.append("yPosition = ").append(yPosition);

            System.out.println(message);

            Thread.sleep(1000);
        }
    }
}
