#include <iostream>
#include <fstream>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <string>

using namespace std;

// 全局终止标志
volatile sig_atomic_t stop_flag = 0;

// 常量定义（基于图片中公式与用户数据）
const uint64_t C = 43748131634ULL;  // 图片公式中的核心常数（sin函数分母）
const double PI = M_PI;             // 圆周率（需包含<cmath>并启用_USE_MATH_DEFINES）
const uint64_t CENTER_MIN_RADIUS = 1000ULL;  // 中心岛最小半径（方块数）
const uint64_t CENTER_MAX_RADIUS = 370727ULL; // 中心岛最大半径（方块数）
const double AREA_ERROR_THRESHOLD = 0.01;    // 面积误差允许阈值（模拟游戏精度）

// 文件流（追加模式）
ofstream outfile("Minecraft_end_world_realistic.txt", ios::app);

// 信号处理（Ctrl+C终止）
void handle_sigint(int sig) {
    stop_flag = 1;
}

// 计算并输出环参数（考虑游戏生成误差）
void simulate_end_generation() {
    outfile << "========== 末地环真实生成模拟（方块单位） ==========\n";
    outfile << "【基础参数】\n";
    outfile << "公式常数 C: "<< C << "\n";
    outfile << "中心岛半径范围: " << CENTER_MIN_RADIUS << " ~ " << CENTER_MAX_RADIUS << " 方块\n\n";

    // 1. 中心岛参数计算（圆形区域）
    outfile << "【中心岛（初始圆形）】\n";
    double center_exact_area = PI * CENTER_MAX_RADIUS * CENTER_MAX_RADIUS;
    uint64_t center_approx_area = static_cast<uint64_t>(round(center_exact_area));  // 近似面积（四舍五入）
    double center_circumference = 2 * PI * CENTER_MAX_RADIUS;                       // 周长（浮点）
    uint64_t center_approx_circumference = static_cast<uint64_t>(round(center_circumference));  // 近似周长（四舍五入）
    
    outfile << "中心岛精确面积: " << center_exact_area << " 方块²（理论值）\n";
    outfile << "中心岛近似面积: " << center_approx_area << " 方块²（游戏计数）\n";
    outfile << "中心岛精确周长: " << center_circumference << " 方块（理论值）\n";
    outfile << "中心岛近似周长: " << center_approx_circumference << " 方块（游戏计数）\n\n";

    // 2. 环状结构计算（从第1环开始）
    outfile << "【环状结构（同心圆环）】\n";
    uint64_t prev_outer = CENTER_MAX_RADIUS;  // 上一环外半径（初始为中心岛最大半径）
    uint64_t ring_num = 1;                    // 环编号（从1开始）

    while (!stop_flag) {
        // 理论半径计算（基于图片公式 sin((x²+y²)/C) 的周期性）
        double theoretical_r = sqrt((C * ring_num) / PI);  // 理论浮点半径
        double error = (theoretical_r - floor(theoretical_r)) * 100;  // 理论误差百分比
        
        // 游戏实际生成半径（向下取整，模拟方块对齐）
        uint64_t actual_outer = static_cast<uint64_t>(floor(theoretical_r));
        
        // 检测异常（半径回退或误差过大）
 /*       if (actual_outer < prev_outer) {
            outfile << "【警告】环" << ring_num << "半径回退（" << actual_outer 
                   << " < " << prev_outer << "），触发游戏生成限制，终止计算\n";
            break;
        }
        if (error > 5.0) {  // 理论误差超过5%视为异常（游戏引擎精度限制）
            outfile << "【提示】环" << ring_num << "理论误差过大（" << error 
                    << "%），可能受游戏数值精度影响\n";
        }
*/
        // 计算环参数
        uint64_t ring_width = actual_outer - prev_outer;  // 环宽度（方块数）
        double ring_area = PI * (pow(actual_outer, 2) - pow(prev_outer, 2));  // 环面积（理论值）
        double area_error = abs(ring_area - AREA_ERROR_THRESHOLD) / AREA_ERROR_THRESHOLD;  // 面积误差比

        // 输出到文件（保留关键数据）
        outfile << "环 " << ring_num << ":\n";
        outfile << "  理论外半径: " << theoretical_r << " 方块（浮点计算）\n";
        outfile << "  实际外半径: " << actual_outer << " 方块（游戏方块对齐）\n";
        outfile << "  内半径: " << prev_outer << " 方块\n";
        outfile << "  环宽度: " << ring_width << " 方块\n";
        outfile << "  理论面积: " << ring_area << " 方块²（π(r²-r₀²)）\n";
        outfile << "  面积误差: " << area_error*100 << "%（与恒定面积偏差）\n";
        outfile << "  理论误差: " << error << "%（理论半径与实际取整差）\n\n";

        // 更新参数
        prev_outer = actual_outer;
        ring_num++;

        // 进度提示（每100环输出一次）
        if (ring_num % 100 == 0) {
            cout << "已计算到环 " << ring_num << "（总环数：" << ring_num << "）" << endl;
        }

        // 防止无限循环（半径超过游戏实际生成上限，假设为1e18方块）
        if (actual_outer > 1e18) {
            outfile << "【提示】半径超过游戏生成上限（1e18方块），终止计算\n";
            break;
        }
    }

    outfile << "========== 模拟终止（环 " << ring_num << "） ==========\n";
    outfile.close();
}

int main() {
    // 启用数学常数PI（MSVC需要此宏）
#ifdef _MSC_VER
    #define _USE_MATH_DEFINES
    #include <cmath>
#else
    #define M_PI 3.14159265358979323846
#endif

    // 注册信号处理
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    cout << "开始模拟末地环真实生成（方块单位），按Ctrl+C终止...\n";
    simulate_end_generation();

    cout << "结果已保存至 Minecraft_end_world_realistic.txt\n";
    return 0;
}
