#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Window;  // Forward declaration if necessary

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type { Press, Release };
	public:
		Event( Type type, unsigned char code ) noexcept;
		bool IsPress() const noexcept;
		bool IsRelease() const noexcept;
		unsigned char GetCode() const noexcept;
    private:
		Type type;
		unsigned char code;
	};
public:
	Keyboard();
	Keyboard( const Keyboard& ) = delete;
	Keyboard& operator=( const Keyboard& ) = delete;
	bool KeyIsPressed( unsigned char keycode ) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed( unsigned char keycode ) noexcept;
	void OnKeyReleased( unsigned char keycode ) noexcept;
	void OnChar( char character ) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer( std::queue<T>& buffer ) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = true;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};
