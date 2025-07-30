import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.function.Function;

public class EndSimulation {
    // 末地环核心参数（基于图片与MC Wiki数据）
    private static final double OMEGA = 0.0000000000228580929;  // sin²函数频率系数
    private static final long RING_START = 370720L;            // 起始环半径
    private static final long RING_END = 29996880L;            // 结束环半径
    private static final int TOTAL_RINGS = 3274;               // 总缺失环数

    public static void main(String[] args) {
        // 生成PNG图像
        generatePNG("end_world.png", 8000, 8000, true);
        // 生成ASCII字符图
        generateAsciiArt("end_world.txt", 100, 100);
    }

    // 生成PNG图像
    public static void generatePNG(String filename, int width, int height, boolean showMissing) {
        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2d = image.createGraphics();

        // 绘制背景（正常地形）
        g2d.setColor(new Color(50, 150, 255));  // 天空蓝
        g2d.fillRect(0, 0, width, height);

        // 绘制末地环缺失区域
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // 将像素坐标映射到世界坐标（以原点为中心）
                double worldX = (x - width / 2.0) / 100.0;  // 缩放因子100像素=1格
                double worldZ = (y - height / 2.0) / 100.0;
                double radius = Math.sqrt(worldX * worldX + worldZ * worldZ);

                // 判断是否在缺失环内
                if (isMissingRing(radius)) {
                    if (showMissing) {
                        g2d.setColor(new Color(0, 0, 0, 0));  // 透明缺失区域
                    } else {
                        g2d.setColor(new Color(100, 100, 100));  // 灰色实心替代
                    }
                    g2d.fillRect(x, y, 1, 1);
                }
            }
        }
        g2d.dispose();

        // 保存图像
        try {
            ImageIO.write(image, "png", new File(filename));
            System.out.println("PNG图像生成成功：" + filename);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 生成ASCII字符图
    public static void generateAsciiArt(String filename, int cols, int rows) {
        StringBuilder sb = new StringBuilder();
        double scaleX = (RING_END * 2) / cols;  // 水平缩放
        double scaleY = (RING_END * 2) / rows; // 垂直缩放

        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                // 计算世界坐标
                double worldX = (x - cols / 2.0) * scaleX;
                double worldZ = (y - rows / 2.0) * scaleY;
                double radius = Math.sqrt(worldX * worldX + worldZ * worldZ);

                // 计算地形密度（基于sin²函数）
                double density = 1 - Math.pow(Math.sin(radius * radius * OMEGA), 2);
                char c = getCharFromDensity(density);
                sb.append(c);
            }
            sb.append("\n");
        }

        // 保存字符图
        try (FileWriter writer = new FileWriter(filename)) {
            writer.write(sb.toString());
            System.out.println("ASCII字符图生成成功：" + filename);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 判断是否属于缺失环
    private static boolean isMissingRing(double radius) {
        if (radius < RING_START || radius > RING_END) return false;
        double xSq = radius * radius;
        double sinVal = Math.pow(Math.sin(xSq * OMEGA), 2);
        return sinVal > 1e-9;  // 阈值判断
    }

    // 根据密度选择字符
    private static char getCharFromDensity(double density) {
        if (density < 0.3) return ' ';
        if (density < 0.6) return '.';
        if (density < 0.9) return '*';
        return '#';
    }
}
