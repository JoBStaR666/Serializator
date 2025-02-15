#include <iostream>
#include <vector>
#include <fstream>
#include "SerializeType.h"
#include <variant>



class IntegerType;
class FloatType;
class StringType;
class VectorType;
class Any;


class IntegerType : public Serialazible<IntegerType, uint64_t, TypeId::Uint> {
public:
	IntegerType() = default;
	IntegerType(uint64_t val) noexcept :Serialazible(std::move(val)) {};
};

class FloatType : public Serialazible<FloatType, double, TypeId::Float> {
public:
	FloatType() = default;
	FloatType(double val) noexcept :Serialazible(std::move(val)) {};
};

class StringType : public Serialazible<StringType, std::string, TypeId::String> {
public:
	StringType() = default;
	StringType(std::string&& val) noexcept :Serialazible(std::move(val)) {};
	StringType(const std::string& val) noexcept :Serialazible(val) {};

};

class VectorType : public Serialazible<VectorType, std::vector<Any>, TypeId::Vector> {
public:
	VectorType() = default;
	VectorType(std::initializer_list<Any>&& list) noexcept : Serialazible(std::move(list)) {};

	template<typename Arg, std::enable_if_t<std::is_constructible_v<Any, Arg>, bool> = true>
	void push_back(Arg&& _val)
	{
		value.emplace_back(std::forward<Arg>(_val));
	}
};


class Any {
private:
	TypeId _typeId;
	Buffer _dataSerialized;
	std::variant< IntegerType, FloatType, StringType, VectorType> _store;
public:
	template<typename Args, std::enable_if_t<is_have_serialize_v<Args>, bool> = true>
	Any(Args&& value) noexcept : _typeId(value.getType()), _dataSerialized(value.serialize()), _store(std::forward<Args>(value))
	{}
	Any() = default;
	void serialize(Buffer& _buff) const
	{
		if (_dataSerialized.size())
			_buff.insert(_buff.end(), _dataSerialized.begin(), _dataSerialized.end());
	}

	Buffer::const_iterator deserialize(Buffer::const_iterator _begin, Buffer::const_iterator _end)
	{
		/*	const uint64_t* typeValue = reinterpret_cast<const uint64_t*>(&(*_begin));
			switch (_typeId = static_cast<TypeId>(*typeValue), _typeId)
			{
			case TypeId::Uint:
				inplaseToStore(IntegerType::deserialize(_begin, _end));
				break;
			case TypeId::Float:
				inplaseToStore(FloatType::deserialize(_begin, _end));
				break;
			case TypeId::String:
				inplaseToStore(StringType::deserialize(_begin, _end));
				break;
			case TypeId::Vector:
				inplaseToStore(VectorType::deserialize(_begin, _end));
				break;
			default:
				break;
			}*/
		return _begin;
	};

	TypeId getPayloadTypeId() const
	{
		return _typeId;
	}

	template<typename Type, std::enable_if_t<is_have_serialize_v<Type>, bool> = true>
	auto& getValue() const
	{
		if (_dataSerialized.size())
			return std::get<Type>(_store);
		throw std::bad_variant_access();
	}

	template<TypeId kId>
	auto& getValue() const
	{
		if (_dataSerialized.size())
			return std::get<static_cast<size_t>(kId)>(_store);
		throw std::bad_variant_access();
	}

	bool operator== (const Any& _o) const
	{
		if (this == &_o)
			return true;
		return _dataSerialized == _o._dataSerialized;
	}

private:
	template<typename T>
	void inplaseToStore(T&& value)
	{
		_dataSerialized = value.serialize();
		_store = std::forward<T>(value);
	}
};






class Serializator {
public:
	template<typename Arg>
	void push(Arg&& _val) {
		_dataStorage.emplace_back(std::forward<Arg>(_val));
	}

	Buffer serialize() const
	{
		Buffer returnBuffer;
		returnBuffer.reserve(_dataStorage.size() * sizeof(Any));
		for (const auto& item : _dataStorage)
		{

		}
	}

	static std::vector<Any> deserialize(const Buffer& _val);

	const std::vector<Any>& getStorage() const;
private:
	std::vector<Any> _dataStorage;
};


int main()
{
	//FloatType f{ 11 };
	//Any ai{ i };
	//Any af{ f };
	//VectorType va{ ai, af };
	//Any v{ va };
	//try {
	//	auto value2 = v.getValue<TypeId::Vector>();
	//	int i22 = 100;
	//	++i22;
	//}
	//catch (std::exception& e)
	//{
	//	std::cout << e.what() << std::endl;
	//}
	//catch (...)
	//{

	//}
	Any aa(IntegerType(10));
	int i = 10;
	std::cin >> i;
	std::cout << aa.getValue<(TypeId)i>();
	return 0;
}