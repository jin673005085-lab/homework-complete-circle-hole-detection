#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iterator>
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

int main(int argc, char** argv) {
    if (argc != 2 && argc != 4) {
        std::cerr << "Usage: circle_holes <image> [min_radius max_radius]\n"
                     "       circle_holes --self-test\n";
        return 1;
    }
    int minRadius = 0, maxRadius = 0;
    if (argc == 4) {
        try {
            size_t a, b;
            minRadius = std::stoi(argv[2], &a);
            maxRadius = std::stoi(argv[3], &b);
            if (a != std::string(argv[2]).size() || b != std::string(argv[3]).size() ||
                minRadius < 0 || maxRadius <= minRadius) throw std::invalid_argument("radius");
        } catch (const std::exception&) {
            std::cerr << "Invalid radius range; require 0 <= min_radius < max_radius.\n";
            return 1;
        }
    }
    const bool selfTest = std::string(argv[1]) == "--self-test";
    cv::Mat image;
    if (selfTest) {
        image = cv::Mat(240, 240, CV_8UC3, cv::Scalar(255, 255, 255));
        cv::circle(image, {120, 120}, 40, cv::Scalar(0, 0, 0), -1);
        minRadius = 30; maxRadius = 50;
    } else {
        std::filesystem::path path = argv[1];
#ifdef _WIN32
        int wideArgc = 0;
        wchar_t** wideArgv = CommandLineToArgvW(GetCommandLineW(), &wideArgc);
        if (!wideArgv || wideArgc < 2) {
            if (wideArgv) LocalFree(wideArgv);
            std::cerr << "Cannot read command line.\n";
            return 1;
        }
        path = wideArgv[1];
        LocalFree(wideArgv);
#endif
        std::ifstream file(path, std::ios::binary);
        std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)), {});
        if (!bytes.empty()) image = cv::imdecode(bytes, cv::IMREAD_COLOR);
        if (image.empty()) { std::cerr << "Cannot read image: " << argv[1] << '\n'; return 1; }
    }
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, {9, 9}, 2);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 20, 100, 30, minRadius, maxRadius);
    int count = 0;
    for (const auto& c : circles) {
        const int x = cvRound(c[0]), y = cvRound(c[1]), r = cvRound(c[2]);
        // shortcut: bounding-box check cannot detect occluded arcs; use contour validation for occlusion.
        if (x - r <= 0 || y - r <= 0 || x + r >= image.cols - 1 || y + r >= image.rows - 1) continue;
        ++count;
        std::cout << count << ": center=(" << x << ", " << y << "), radius=" << r << " px\n";
        cv::circle(image, {x, y}, r, {0, 0, 255}, 2);
        cv::circle(image, {x, y}, 2, {0, 255, 0}, -1);
    }
    std::cout << "Total: " << count << '\n';
    if (selfTest) return count == 1 ? 0 : 1;
    if (!cv::imwrite("circle_result.png", image)) {
        std::cerr << "Cannot write circle_result.png\n"; return 1;
    }
    std::cout << "Saved: circle_result.png\n";
    return 0;
}
