#include <core/Utils.hpp>
#include <windows.h>
int main() {
    auto r = load_file_async(__FILE__);
    std::cout << __FILE__ << ": " <<r.get().data();
    return 0;
}