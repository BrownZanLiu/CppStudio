/**
 * Copyright: brown.liuzan@outlook.com
 */

#ifndef LIUZAN_DYNAMIC_ASSERT_H
#define LIUZAN_DYNAMIC_ASSERT_H



#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <format>

namespace liuzan {

/**
 * Design goals:
 * 1) For a condition as expected, do nothing and make optimization-out possilbe.
 * 2) For a product source code tree, it's desired to support different hebaviors with different
 * building configurations. Expected building configurations:
 *     DEBUG, panic with an unexpected condition
 *     RELEASE, throw exception with an unexpected condition
 */

enum class DynamicAssertMode {
	PANIC = 0,  // std::cerr messages and std::abort()
	THROW_EXCEPTION = 1,  // throw liuzan::DynamicAssertionFailure
};

class DynamicAssertionFailure: public std::exception
{
public:
	explicit DynamicAssertionFailure(std::string const &msg): mReason(msg) {}

	explicit DynamicAssertionFailure(std::string &&msg): mReason(std::move(msg)) {}

	char const * what() const noexcept override
	{
		return mReason.c_str();
	}
private:
	std::string mReason;
};

template<DynamicAssertMode mode, typename... Ts>
void DynamicAssertInternal(bool isAsExpected, char const *msgFmt, Ts... args)
{
	if (isAsExpected) [[likely]] {
		return;
	}

	std::string vMsg = std::format("DynamicAssertionFailure: ");
	vMsg += std::vformat(msgFmt, std::make_format_args(args...));
	if constexpr (mode == DynamicAssertMode::PANIC) {
		std::cerr << vMsg << std::endl;
		std::abort();
	} else if constexpr (mode == DynamicAssertMode::THROW_EXCEPTION) {
		std::cerr << vMsg << std::endl;
		throw DynamicAssertionFailure(std::move(vMsg));
	} else {
		throw std::logic_error("Invalid DynamicAssertMode");
	}
}

template<DynamicAssertMode mode>
void DynamicAssertInternal(bool isAsExpected, char const *msg)
{
	if (isAsExpected) [[likely]] {
		return;
	}

	std::string vMsg = std::format("DynamicAssertionFailure: {}", msg);
	if constexpr (mode == DynamicAssertMode::PANIC) {
		std::cerr << vMsg << std::endl;
		std::abort();
	} else if constexpr (mode == DynamicAssertMode::THROW_EXCEPTION) {
		std::cerr << vMsg << std::endl;
		throw DynamicAssertionFailure(vMsg);
	} else {
		throw std::logic_error("Invalid DynamicAssertMode");
	}
}

template<DynamicAssertMode mode>
void DynamicAssertInternal(bool isAsExpected)
{
	if (isAsExpected) [[likely]] {
		return;
	}

	std::string vMsg = std::format("DynamicAssertionFailure!");
	if constexpr (mode == DynamicAssertMode::PANIC) {
		std::cerr << vMsg << std::endl;
		std::abort();
	} else if constexpr (mode == DynamicAssertMode::THROW_EXCEPTION) {
		std::cerr << vMsg << std::endl;
		throw DynamicAssertionFailure(vMsg);
	} else {
		throw std::logic_error("Invalid DynamicAssertMode");
	}
}

#if defined(DEBUG) || defined(debug) || defined(Debug)
template<typename... Ts>
inline void DynamicAssert(bool isAsExpected, char const *msg, Ts... args)
{
	DynamicAssertInternal<DynamicAssertMode::PANIC>(isAsExpected, msg, args...);
}
inline void DynamicAssert(bool isAsExpected, char const *msg)
{
	DynamicAssertInternal<DynamicAssertMode::PANIC>(isAsExpected, msg);
}
inline void DynamicAssert(bool isAsExpected)
{
	DynamicAssertInternal<DynamicAssertMode::PANIC>(isAsExpected);
}
#else
template<typename... Ts>
inline void DynamicAssert(bool isAsExpected, char const *msg, Ts... args)
{
	DynamicAssertInternal<DynamicAssertMode::THROW_EXCEPTION>(isAsExpected, msg, args...);
}
inline void DynamicAssert(bool isAsExpected, char const *msg)
{
	DynamicAssertInternal<DynamicAssertMode::THROW_EXCEPTION>(isAsExpected, msg);
}
inline void DynamicAssert(bool isAsExpected)
{
	DynamicAssertInternal<DynamicAssertMode::THROW_EXCEPTION>(isAsExpected);
}
#endif

}  // namespace liuzan



#endif  // LIUZAN_DYNAMIC_ASSERT_H
