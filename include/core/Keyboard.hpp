#pragma once

class CWindow;  // Forward declaration if necessary

class Keyboard
{
	friend class CWindow;
private:
	class Event
	{
	public:
		enum class Type { Press, Release };
	public:
		Event( Type type, unsigned char code ) noexcept;
		auto IsPress() const noexcept 	-> bool ;
		auto IsRelease() const noexcept -> bool ;
		auto Code() const noexcept 	-> unsigned char;
    private:
		Type type;
		unsigned char code;
	};
public:
	Keyboard();
	Keyboard( const Keyboard& ) = delete;
	auto operator=( const Keyboard& ) -> Keyboard& = delete;
	auto ReadKey() noexcept 								 	-> std::optional<Event> ;
	auto ReadChar() noexcept 								 	-> std::optional<unsigned char> ;

	auto IsKeyPressed( unsigned char keycode ) const noexcept 	-> bool ;
	auto IsKeyReleased(unsigned char keycode) const noexcept    -> bool ;
	auto IsKeyDown(unsigned char keycode) const noexcept 		-> bool ;
	auto IsKeyUp(unsigned char keycode) const noexcept 			-> bool ;

	auto FlushKey() noexcept 								 	-> void ;
	auto FlushChar() noexcept 									-> void ;
	auto Flush() noexcept 										-> void ;
	auto EnableAutorepeat() noexcept 							-> void ;
	auto DisableAutorepeat() noexcept 							-> void ;
	auto AutorepeatIsEnabled() const noexcept 					-> bool ;
	auto UpdatePrevState() noexcept 							-> void;

private:
	auto OnKeyPressed( unsigned char keycode ) noexcept  		-> void ;
	auto OnKeyReleased( unsigned char keycode ) noexcept 		-> void ;
	auto OnChar( unsigned char character ) noexcept 			-> void ;
	auto ClearState() noexcept 									-> void ;
	template<typename Container>
	requires requires(Container c) { c.size(); c.pop(); }
	static auto TrimBuffer( Container& buffer ) noexcept	-> void;

private:
	static constexpr uint32_t nKeys = 256u;
	static constexpr uint32_t bufferSize = 16u;
	bool autorepeatEnabled = true;
	std::bitset<nKeys> keystates;
	std::bitset<nKeys> prevKeyStates;
	std::queue<Event> keybuffer;
	std::queue<unsigned char> charbuffer;

	FOR_TEST
};
