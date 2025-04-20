#include <core/Utils.hpp>
#include <windows.h>
int main() {
    auto r = load_file_async(__FILE__).get();
    if(r.has_value()) return 0;
    else return 1;
}