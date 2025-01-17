#include <core/Keyboard.hpp>
// Event/////////////////////////////////////////////////
Keyboard::Event::Event( Type type, unsigned char code ) noexcept
    			:
    			type( type ),
    			code( code )
    		{}
auto Keyboard::Event::IsPress() const noexcept -> bool
{
	return type == Type::Press;
}
auto Keyboard::Event::IsRelease() const noexcept -> bool
{
	return type == Type::Release;
}
auto Keyboard::Event::GetCode() const noexcept -> unsigned char
{
	return code;
}
/////////////////////////////////////////////////////////

//KeyBoard//////////////////////////////////////////////
Keyboard::Keyboard(){}
auto Keyboard::KeyIsPressed( unsigned char keycode ) const noexcept -> bool
{
	return keystates[keycode];
}

auto Keyboard::ReadKey() noexcept -> std::optional<Keyboard::Event>
{
	if( keybuffer.size() > 0u )
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}

auto Keyboard::KeyIsEmpty() const noexcept -> bool
{
	return keybuffer.empty();
}

auto Keyboard::ReadChar() noexcept -> std::optional<char>
{
	if( charbuffer.size() > 0u )
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	return {};
}

auto Keyboard::CharIsEmpty() const noexcept -> bool
{
	return charbuffer.empty();
}

auto Keyboard::FlushKey() noexcept -> void
{
	keybuffer = std::queue<Event>();
}

auto Keyboard::FlushChar() noexcept -> void
{
	charbuffer = std::queue<char>();
}

auto Keyboard::Flush() noexcept -> void
{
	FlushKey();
	FlushChar();
}

auto Keyboard::EnableAutorepeat() noexcept -> void
{
	autorepeatEnabled = true;
}

auto Keyboard::DisableAutorepeat() noexcept -> void
{
	autorepeatEnabled = false;
}

auto Keyboard::AutorepeatIsEnabled() const noexcept -> bool
{
	return autorepeatEnabled;
}

auto Keyboard::OnKeyPressed( unsigned char keycode ) noexcept -> void
{
	keystates[keycode] = true;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Type::Press,keycode ) );
	TrimBuffer( keybuffer );
}

auto Keyboard::OnKeyReleased( unsigned char keycode ) noexcept -> void
{
	keystates[keycode] = false;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Type::Release,keycode ) );
	TrimBuffer( keybuffer );
}

auto Keyboard::OnChar( char character ) noexcept -> void
{
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}

auto Keyboard::ClearState() noexcept -> void
{
	keystates.reset();
}

template<typename T>
auto Keyboard::TrimBuffer( std::queue<T>& buffer ) noexcept -> void
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}
/////////////////////////////////////////////////////////