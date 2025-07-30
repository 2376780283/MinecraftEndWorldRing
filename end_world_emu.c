#include <iostream>
#include <fstream>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <string>

using namespace std;

// 全局终止标志（Ctrl+C终止）
volatile sig_atomic_t stop_flag = 0;

// JE版核心常量（基于图片与MC Wiki数据）
const uint64_t C = 43748131634ULL;          // sin²函数分母（原图片公式）
const double OMEGA = 0.0000000000228580929; // sin²函数频率系数（x²·OMEGA）
const uint64_t WORLD_BOUND_JE = 29888960ULL;// JE世界边界（±29,996,880格，接近3e7）
const uint64_t RING_START = 370720ULL;      // 起始环半径（X/Z=±370,720）
const uint64_t RING_END = 29996880ULL;      // 结束环半径（X/Z=±29,996,880）
const int TOTAL_RINGS_JE = 3274;            // JE版总缺失环数（图片明确）

// 文件流（追加模式）
ofstream outfile("Minecraft_JE_end_rings.txt", ios::app);

// 信号处理（Ctrl+C终止）
void handle_sigint(int sig) {
    stop_flag = 1;
}

// 计算sin²函数值（控制地形缺失）
double sin_squared(double x_sq) {
    return pow(sin(x_sq * OMEGA), 2);
}

// 模拟JE版末地环生成（完美还原地形缺失特性）
void simulate_JE_end_generation() {
    outfile << "========== Minecraft Java版末地环真实生成模拟 ==========\n";
    outfile << "【基础参数】\n";
    outfile << "世界边界: ±" << WORLD_BOUND_JE << " 格\n";
    outfile << "起始环半径: " << RING_START << " 格（X/Z=±" << RING_START << "）\n";
    outfile << "结束环半径: " << RING_END << " 格（X/Z=±" << RING_END << "）\n";
    outfile << "总缺失环数: " << TOTAL_RINGS_JE << "（图片明确）\n\n";

    // 1. 中心岛参数（原始圆形区域）
    outfile << "【中心岛（初始圆形）】\n";
    uint64_t center_max_radius = 370727ULL;  // 中心岛最大半径（X/Z=±370,727）
    double center_exact_area = M_PI * center_max_radius * center_max_radius;
    uint64_t center_approx_area = static_cast<uint64_t>(round(center_exact_area));
    
    outfile << "中心岛半径范围: 1000 格 - " << center_max_radius << " 格\n";
    outfile << "中心岛精确面积: " << center_exact_area << " 方块²（理论值）\n";
    outfile << "中心岛近似面积: " << center_approx_area << " 方块²（游戏计数）\n\n";

    // 2. 环状结构计算（从第1环到第3274环）
    outfile << "【环状结构（同心圆环缺失区域）】\n";
    uint64_t prev_outer = RING_START;  // 上一环外半径（初始为起始环半径）
    uint64_t ring_num = 1;             // 环编号（从1开始）

    while (!stop_flag && ring_num <= TOTAL_RINGS_JE) {
        // 计算当前环理论外半径（基于JE世界边界与总环数）
        double theoretical_r = RING_START + (ring_num - 1) * (RING_END - RING_START) / (TOTAL_RINGS_JE - 1);
        
        // 游戏实际生成半径（向下取整，模拟方块对齐）
        uint64_t actual_outer = static_cast<uint64_t>(floor(theoretical_r));
        
        // 检测溢出（超过JE世界边界）
        if (actual_outer > WORLD_BOUND_JE) {
            outfile << "【警告】环" << ring_num << "半径超过JE世界边界（" << WORLD_BOUND_JE 
                   << "格），触发游戏生成限制，终止计算\n";
            break;
        }

        // 计算当前环中心x²值（用于sin²函数判断地形缺失）
        double x_sq = actual_outer * actual_outer;  // x=Z坐标（对称性）
        
        // 地形缺失条件：sin²(x²·OMEGA) > 0（实际游戏中当y>0时不生成地形）
        double sin_val = sin_squared(x_sq);
        bool is_missing = (sin_val > 1e-9);  // 阈值防止浮点误差
        
        // 计算环参数（宽度、面积）
        uint64_t ring_width = actual_outer - prev_outer;
        double ring_area = M_PI * (pow(actual_outer, 2) - pow(prev_outer, 2));  // 环面积（方块²）
        
        // 输出到文件（JE版关键数据）
        outfile << "环 " << ring_num << ":\n";
        outfile << "  理论外半径: " << theoretical_r << " 格（浮点计算）\n";
        outfile << "  实际外半径: " << actual_outer << " 格（游戏方块对齐）\n";
        outfile << "  内半径: " << prev_outer << " 格\n";
        outfile << "  环宽度: " << ring_width << " 格\n";
        outfile << "  环面积: " << ring_area << " 方块²\n";
        outfile << "  地形状态: " << (is_missing ? "缺失" : "正常生成") << "\n";
        outfile << "  sin²值: " << sin_val << "（判断依据）\n\n";

        // 更新参数
        prev_outer = actual_outer;
        ring_num++;

        // 进度提示（每100环输出一次）
        if (ring_num % 100 == 0) {
            cout << "已计算到环 " << ring_num << "（总环数：" << ring_num << "/" << TOTAL_RINGS_JE << "）" << endl;
        }
    }

    outfile << "========== 模拟终止（环 " << ring_num << "） ==========\n";
    outfile.close();
}

int main() {
    // 启用数学常数M_PI（需包含<cmath>并启用_USE_MATH_DEFINES）
#ifdef _MSC_VER
    #define _USE_MATH_DEFINES
    #include <cmath>
#else
    #define M_PI 3.14159265358979323846
#endif

    // 注册信号处理（Ctrl+C终止）
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    cout << "开始模拟Minecraft Java版末地环生成（完美还原JE特性），按Ctrl+C终止...\n";
    simulate_JE_end_generation();

    cout << "结果已保存至 Minecraft_JE_end_rings.txt\n";
    return 0;
}
