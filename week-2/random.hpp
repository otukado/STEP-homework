#include <cstdint>

namespace otukado {


struct xor_shift {
    uint32_t y;

    xor_shift(uint32_t seed = 0) { this->seed(seed); }

    auto min() { return 0; }
    auto max() { return 4294967295; }

    void seed(uint32_t seed) {
        this->y = 2463534242 ^ seed;
    }

    auto operator()() {
        this->y = this->y ^ (this->y << 13);
        this->y = this->y ^ (this->y >> 17);
        return this->y = this->y ^ (this->y << 5); 
    }

    // 0-1 の値を返す
    auto random() {
        return static_cast<double>(this->operator()()) / this->max();
    }
};


} // namespace otukado