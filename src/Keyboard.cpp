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
auto Keyboard::Event::Code() const noexcept -> unsigned char
{
	return code;
}
/////////////////////////////////////////////////////////

//KeyBoard//////////////////////////////////////////////
Keyboard::Keyboard(){
	keystates.reset();
	prevKeyStates.reset();
}
auto Keyboard::IsKeyDown(unsigned char keycode) const noexcept -> bool
{
    return keystates[keycode];
}
auto Keyboard::IsKeyUp(unsigned char keycode) const noexcept -> bool
{
    return !keystates[keycode];
}

auto Keyboard::IsKeyPressed( unsigned char keycode ) const noexcept -> bool
{
	return keystates[keycode] && !prevKeyStates[keycode];
}

auto Keyboard::IsKeyReleased(unsigned char keycode) const noexcept -> bool
{
    return !keystates[keycode] && prevKeyStates[keycode];
}

auto Keyboard::ReadKey() noexcept -> std::optional<Keyboard::Event>
{
	if( !keybuffer.empty() )
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}


auto Keyboard::ReadChar() noexcept -> std::optional<unsigned char>
{
	if( !charbuffer.empty())
	{
		auto charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	return {};
}

auto Keyboard::FlushKey() noexcept -> void
{
	keybuffer = {};
}

auto Keyboard::FlushChar() noexcept -> void
{
	charbuffer = {};
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

auto Keyboard::UpdatePrevState() noexcept -> void
{
    prevKeyStates = keystates;
}

auto Keyboard::OnKeyPressed( unsigned char keycode ) noexcept -> void
{
	keystates[keycode] = true;
	keybuffer.emplace( Keyboard::Event::Type::Press, keycode );
	TrimBuffer( keybuffer );
}

auto Keyboard::OnKeyReleased( unsigned char keycode ) noexcept -> void
{
	keystates[keycode] = false;
	keybuffer.emplace( Keyboard::Event::Type::Release, keycode );
	TrimBuffer( keybuffer );
}

auto Keyboard::OnChar( unsigned char character ) noexcept -> void
{
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}

auto Keyboard::ClearState() noexcept -> void
{
	keystates.reset();
}

template<typename Container>
requires requires(Container c){ c.size(); c.pop(); }
auto Keyboard::TrimBuffer( Container& buffer ) noexcept -> void
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}
/////////////////////////////////////////////////////////
