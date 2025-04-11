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
	auto ReadChar() noexcept 								 	-> std::optional<char> ;
	auto KeyIsPressed( unsigned char keycode ) const noexcept 	-> bool ;
	auto KeyIsEmpty() const noexcept 						 	-> bool ;
	auto FlushKey() noexcept 								 	-> void ;
	auto CharIsEmpty() const noexcept 							-> bool ;
	auto FlushChar() noexcept 									-> void ;
	auto Flush() noexcept 										-> void ;
	auto EnableAutorepeat() noexcept 							-> void ;
	auto DisableAutorepeat() noexcept 							-> void ;
	auto AutorepeatIsEnabled() const noexcept 					-> bool ;
private:
	auto OnKeyPressed( unsigned char keycode ) noexcept  		-> void ;
	auto OnKeyReleased( unsigned char keycode ) noexcept 		-> void ;
	auto OnChar( char character ) noexcept 						-> void ;
	auto ClearState() noexcept 									-> void ;
	template<typename T>
	static auto TrimBuffer( std::queue<T>& buffer ) noexcept	-> void;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = true;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};
