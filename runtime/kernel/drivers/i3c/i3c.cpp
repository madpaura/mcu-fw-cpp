// Licensed under the Apache-2.0 license
// I3C driver implementation

#include <cstdint>

namespace caliptra {
namespace runtime {
namespace kernel {
namespace drivers {

class I3cDriver {
public:
    explicit I3cDriver(std::uintptr_t base_addr) : base_addr_(base_addr) {}
    
    void init() {}
    
private:
    std::uintptr_t base_addr_;
};

} // namespace drivers
} // namespace kernel
} // namespace runtime
} // namespace caliptra
