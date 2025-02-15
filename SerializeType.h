#pragma once
#include <cstddef>
#include <type_traits>
#include <string>

using Buffer = std::vector<std::byte>;
using Id = uint64_t;
enum class TypeId : Id {
	Uint,
	Float,
	String,
	Vector
};
template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
std::ostream& operator<<(std::ostream& stream, const T& type)
{
	return stream << static_cast<uint64_t>(type);
}

template<typename T>
struct is_iterable
{
	using false_type = long long;
	using true_type = char;

	template<class U>
	static false_type check(...);
	template<class U>
	static true_type check(int, typename U::const_iterator = U().end());
	static const bool value = sizeof(decltype(check<T>(0))) == sizeof(true_type);
};
template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
struct is_have_serialize
{
	using false_type = long long;
	using true_type = char;

	template<class U>
	static false_type check(...);
	template<class U>
	static true_type check(int, decltype(std::declval<U>().serialize()) = {});
	static const bool value = sizeof(decltype(check<T>(0))) == sizeof(true_type);
};
template <typename T>
constexpr bool is_have_serialize_v = is_have_serialize<T>::value;

template<class T, typename Enable = void>
struct is_vector {
	static bool const value = false;
};

template<template <typename, typename > class T, class _vType, class _aloc_type>
struct is_vector<T<_vType, _aloc_type>> {
	static bool const value = true;
};
template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

template<typename valueType, typename T, std::enable_if_t<sizeof(valueType) == 1, bool> = true>
void vectorInserter(std::vector<valueType>& buf, const T& value)
{
	buf.insert(buf.end(), reinterpret_cast<const valueType*>(&value), reinterpret_cast<const valueType*>(&value + 1));
}

template<typename valueType, typename T, std::enable_if_t<sizeof(valueType) == 1, bool> = true>
void vectorInserter(std::vector<valueType>& buf, const T* valuePtr, size_t length)
{
	buf.insert(buf.end(), reinterpret_cast<const valueType*>(valuePtr), reinterpret_cast<const valueType*>(valuePtr) + length);
}


//CRTP concept
template<typename DerivedType, typename dataValue, TypeId typeId>
class Serialazible
{
private:
	TypeId type{ typeId };
	dataValue value{};
public:
	TypeId getType() const { return typeId; }

	const dataValue& getValue() const { return value; }


	uint64_t getLength() const {
		if constexpr (is_iterable_v<dataValue>)
			return value.size();
		else
			return 0;
	};


	Buffer serialize() const {
		Buffer b;
		if constexpr (is_iterable_v<dataValue>)
		{
			if (auto size = getLength(); size)
			{
				vectorInserter(b, type);
				vectorInserter(b, size);
				if constexpr (is_vector_v<dataValue>)
					for (const auto* anyItem : value)
					{
						anyItem.serialize(b);
					}
				else
				{
					vectorInserter(b, value.data(), size);
				}
			}
		}
		else
		{
			vectorInserter(b, type);
			vectorInserter(b, value);
		}
		return b;
	}


protected:
	Serialazible() = default;
	Serialazible(dataValue&& value) :value(std::forward<dataValue>(value)) {};
	Serialazible(const dataValue& value) :value(value) {}
};


