#pragma once
#include <string>
#include <vector>
#include <optional>
#include "../dict/data_dict.h"

extern "C"
{
	#include <stdio.h>
}

namespace xdcm
{


/// <summary>
/// Represents a dicom element 
/// (group, element) tag.
/// </summary>
class BaseTag
{
public:
	BaseTag() : m_tag(0x00000000) {}
	BaseTag(const int& _tag) : m_tag(_tag) {}

	/*  Comparison Operators   */
	bool operator<(const BaseTag& _lhs) const noexcept
	{
		return this->m_tag < _lhs.m_tag;
	}
	bool operator>(const BaseTag& _lhs) const noexcept
	{
		return this->m_tag > _lhs.m_tag;
	}
	bool operator<=(const BaseTag& _lhs) const noexcept
	{
		return this->m_tag <= _lhs.m_tag;
	}
	bool operator>=(const BaseTag& _lhs) const noexcept
	{
		return this->m_tag >= _lhs.m_tag;
	}
	bool operator==(const BaseTag& _lhs) const noexcept
	{
		return this->m_tag == _lhs.m_tag;
	}
	bool operator!=(const BaseTag& _lhs) const noexcept
	{
		return !(this->m_tag == _lhs.m_tag);
	}
	
	/// <summary>
	/// Shift m_tag 16 bits to the right to get the group number out of the integar
	/// </summary>
	/// <returns>group number</returns>
	const int group() const noexcept
	{
		return m_tag >> 16;
	}
	/// <summary>
	/// AND the m_tag with 0xffff to get the element number out of the integar
	/// </summary>
	/// <returns>element number</returns>
	const int element() const noexcept
	{
		return m_tag & 0xffff;
	}
	/// <summary>
	/// return true if the group number is an odd number
	/// </summary>
	/// <returns>whether or not the tag is a private tag</returns>
	bool is_private() const
	{
		return this->group() % 2 == 1;
	}
	bool is_private_creator() const
	{
		return (this->is_private() && (this->element() < 0x0100) && (this->element() >= 0x0010));
	}
	std::string representation() const
	{
		std::string str("(0000, 0000)");
		sprintf_s(&(*str.begin()), str.capacity(), "(%04x, %04x)", this->group(), this->element());
		return str;
	}
private:
	/// <summary>
	/// Dicom Tag Stored as One Integar ( 0x0000 0000 )
	/// </summary>
	int m_tag;
};


BaseTag tag(const BaseTag& _tag) { return _tag; }

BaseTag tag(const int& _group_number, const int& _elem_number)
{
	if (_group_number > 0xffff || _elem_number > 0xffff)
		throw(std::overflow_error("group number and element number must be less than 0xffff"));

	int _tag = (_group_number << 16) + _elem_number;
	return BaseTag(_tag);
}

BaseTag tag(const std::pair<int, int>& _pair)
{
	if (_pair.first > 0xffff || _pair.second > 0xffff)
		throw(std::overflow_error("group number and element number must be less than 0xffff"));

	int _tag = (_pair.first << 16) + _pair.second;
	return BaseTag(_tag);
}


BaseTag tag(const int& _tag)
{
	if (_tag > 0xffffffff)
		throw(std::overflow_error("tag value too large: overflow detected!"));
	return BaseTag(_tag);
}


BaseTag tag(const std::string& tag_keyword)
{
	unsigned long long_val;
	try
	{
		long_val = std::strtoul(&(*tag_keyword.begin()), nullptr, 16);
		if (long_val > 0xffffffff)
			throw (std::overflow_error("number too large: overflow!"));

	} // will catch in case of value err and overflow error i.e. is either a string keyword or large value
	catch (const std::exception&)
	{
		// return coresponding tag for the tag value

	}

	return BaseTag();

}







}


