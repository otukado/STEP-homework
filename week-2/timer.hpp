#include <chrono>

namespace otukado {


class timer {
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

  public:
    double elapsed_ms() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - start_time).count();
    }
};


}