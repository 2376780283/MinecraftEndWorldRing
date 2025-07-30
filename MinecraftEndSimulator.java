import java.io.FileWriter;
import java.io.IOException;

public class MinecraftEndSimulator {
    // JE版核心常量（基于图片3）
    private static final double OMEGA = 0.0000000000228580929;  // sin²函数频率系数
    private static final long RING_START = 370720L;            // 起始环半径（X/Z=±370720）
    private static final long RING_END = 29996880L;            // 结束环半径（X/Z=±29996880）
    private static final int TOTAL_RINGS = 3274;               // 总缺失环数（图片3明确）

    public static void main(String[] args) {
        try (FileWriter writer = new FileWriter("JE_End_Rings_Realistic.txt")) {
            // 写入基础参数
            writer.write("=== Minecraft Java版末地环真实模拟 ===\n");
            writer.write("总缺失环数: " + TOTAL_RINGS + "\n");
            writer.write("起始半径: ±" + RING_START + " 格\n");
            writer.write("结束半径: ±" + RING_END + " 格\n\n");

            // 模拟每个环的生成
            for (int n = 1; n <= TOTAL_RINGS; n++) {
                // 计算当前环理论外半径（线性映射）
                double theoreticalRadius = RING_START + (n - 1) * (RING_END - RING_START) / (double) (TOTAL_RINGS - 1);
                
                // 游戏实际生成半径（向下取整，模拟方块对齐）
                long actualRadius = (long) Math.floor(theoreticalRadius);
                
                // 计算地形状态（sin²(x²·OMEGA) > 1e-9 则缺失）
                double xSq = actualRadius * actualRadius;
                double sinSqValue = Math.pow(Math.sin(xSq * OMEGA), 2);
                boolean isMissing = sinSqValue > 1e-9;

                // 写入环参数
                writer.write("环 " + n + ":\n");
                writer.write("  理论半径: " + theoreticalRadius + " 格\n");
                writer.write("  实际半径: " + actualRadius + " 格\n");
                writer.write("  地形状态: " + (isMissing ? "缺失" : "正常生成") + "\n");
                writer.write("  sin²值: " + sinSqValue + "\n\n");
            }

            writer.write("=== 模拟完成 ===");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
