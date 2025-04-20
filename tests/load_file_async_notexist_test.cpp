#include <core/Utils.hpp>
#include <windows.h>
int main() {
    auto r = load_file_async("fileNotExist").get();
    if(not r.has_value()) return 0;
    else return 1;
}