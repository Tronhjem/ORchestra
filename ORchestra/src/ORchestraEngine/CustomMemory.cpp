// #include "CustomMemory.h"

// void* operator new(std::size_t size) {
//     std::cout << "Custom new called for size: " << size << " bytes" << std::endl;
//     return std::malloc(size);
// }

// void* operator new[](std::size_t size) {
//     std::cout << "Custom new[] called for size: " << size << " bytes" << std::endl;
//     return std::malloc(size);
// }

// void operator delete(void* ptr) noexcept {
//     std::cout << "Custom delete called" << std::endl;
//     std::free(ptr);
// }

// void operator delete[](void* ptr) noexcept {
//     std::cout << "Custom delete[] called" << std::endl;
//     std::free(ptr);
// }

// extern "C" {
//     void* malloc(std::size_t size) {
//         std::cout << "Custom malloc called for size: " << size << " bytes" << std::endl;
//         return std::malloc(size);
//     }

//     void free(void* ptr) {
//         std::cout << "Custom free called" << std::endl;
//         std::free(ptr);
//     }
// }
