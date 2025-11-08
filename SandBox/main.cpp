
#include <memory>
#include <random>

#include <Engine.hpp>

extern "C" EG_IMPORT_API auto create_game(class APP& app) -> IGame*;

int main() {
    try {
        APP app;
        auto game = create_game(app);
        app.run(game);

    } catch(const Exception& e) {
        debug::print(e.all());

    } catch(const std::exception& e) {
        debug::print(e.what());

    } catch(...) {
        debug::print("Unknown Exception");
    };

    return 0;
}