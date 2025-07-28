#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <csignal>

const double PI = 3.14159265358979323846;
const double C = 43748181634.0;
const double AREA_PER_RING = C * PI * PI;
const std::string OUTPUT_FILE = "Minecraft_end_c.txt";

bool running = true;

// Ctrl+C handler
void signal_handler(int signum) {
    running = false;
    std::cout << "\n程序终止：已接收到 Ctrl+C\n";
}

// 判断是否进入浮点异常区域（30B~34.3B）
bool in_glitch_zone(double r) {
    return (r > 3e10 && r < 3.43e10);
}

// 可视高度估算
double estimate_view_height(double r) {
    return r * 1.15;
}

int main() {
    std::ofstream outfile(OUTPUT_FILE);
    if (!outfile.is_open()) {
        std::cerr << "无法打开输出文件！" << std::endl;
        return 1;
    }

    std::cout << "开始模拟... 按 Ctrl+C 停止\n";

    std::signal(SIGINT, signal_handler);

    int n = 0;
    while (running) {
        double inner_r = std::sqrt(C * PI * (2 * n));
        double outer_r = std::sqrt(C * PI * (2 * n + 1));
        double width = outer_r - inner_r;

        outfile << "[第 " << n << " 环]\n";
        outfile << "内半径: " << std::fixed << std::setprecision(2) << inner_r / 1e6 << " M格\n";
        outfile << "外半径: " << outer_r / 1e6 << " M格\n";
        outfile << "宽度: " << width / 1e6 << " M格\n";
        outfile << "面积: " << AREA_PER_RING / 1e12 << " M格²（固定）\n";
        outfile << "估算可视高度: " << estimate_view_height(outer_r) / 1e6 << " M格\n";
        if (in_glitch_zone(outer_r)) {
            outfile << "⚠️ 注意：当前处于浮点精度异常区域！环可见性混乱\n";
        }
        outfile << "--------------------------------------\n";

        // 同时在控制台也输出
        std::cout << "已输出第 " << n << " 环（" << outer_r / 1e6 << " M格）\n";
        n++;

        // 控制速度：可以加 sleep
        if (n % 10 == 0) std::cout << "(模拟继续中... 按 Ctrl+C 终止)\n";
    }

    outfile.close();
    std::cout << "输出完成，数据保存在 " << OUTPUT_FILE << "\n";
    return 0;
}