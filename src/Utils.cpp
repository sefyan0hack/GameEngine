#include <core/Utils.hpp>
#include <core/Log.hpp>
using namespace std;

future<optional<vector<char>>> load_file_async(const string& filename) {
    return async(launch::async, [filename]() -> optional<vector<char>> {
        ifstream file(filename, ios::binary | ios::ate);
        if (!file) {
            return std::nullopt;
        }
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);

        vector<char> buffer(static_cast<size_t>(size));
        file.read(buffer.data(), size);
        return buffer;
    });
}

vector<string> split(string s, const string& delimiter) {
    if(s.empty()) return {};
    
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

string replace(string s, char c, char with)
{
    if( c == '\0' || with == '\0')
        return s;
    for(auto& ch: s)
        if(ch == c) 
            ch = with;
    return s;
}