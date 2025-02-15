#include <iostream>
#include <vector>
#include <fstream>
#include "SerializeType.h"
#include <any>




//template <typename T>
class IntegerType;
class FloatType;
class StringType;
class VectorType;




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


class Any {
public:
	template<typename Args, std::enable_if_t<is_have_serialize_v<Args>, bool> = true>
	Any(Args&& value) noexcept : _plasmentData(value.serialize())
	{

	}
	void serialize(Buffer& _buff) const
	{
		if (_plasmentData.size())
			_buff.insert(_buff.end(), _plasmentData.begin(), _plasmentData.end());
	}

	Buffer::const_iterator deserialize(Buffer::const_iterator _begin, Buffer::const_iterator _end);

	TypeId getPayloadTypeId() const
	{
		if (_plasmentData.size())
			return *reinterpret_cast<const TypeId*>(&_plasmentData[0]);
	}

	template<typename Type>
	auto& getValue() const
	{
		//some code to return value from _plasmentData;
	}

	template<TypeId kId>
	auto& getValue() const
	{
	}

	bool operator== (const Any& _o) const
	{
		if (this == &_o)
			return true;
		return _plasmentData == _o._plasmentData;
	}

private:
	Buffer _plasmentData;
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

	/*VectorType vec = { IntegerType(1), FloatType(10.f), StringType("aaa") };
	std::cout << vec.getLength() << std::endl;
	std::cout << IntegerType(10).getLength() << std::endl;
	*/

	//Buffer vecByte;
	//TypeId type = TypeId::Float;
	//uint64_t t = 2;
	//sizeof(std::byte);
	//vectorInserter(vecByte, type);
	StringType s = "asdasd";
	IntegerType i = 10;
	auto buff = s.serialize();
	auto buff2 = i.serialize();
	return 0;
}