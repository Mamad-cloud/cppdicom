#pragma once
#ifndef DATA_DICT
#define DATA_DICT

#include <iostream>
#include <iomanip>

#include <thread>
#include <fstream>

#include <string>
#include <regex>

#include <mutex>


namespace xdcm
{

	// Todo: Maybe use wchar_t for compatability with UNICODE
	/// <summary>
	/// 163 bytes struct to make sure all characters are captured
	/// </summary>
	class PublicDictEntry
	{
	public:
		PublicDictEntry(unsigned int _key, const char* _vr, const char* _vm, const char* _name, const char* _retired, const char* _keyword);

		const unsigned int& key() const;


		friend std::ostream& operator<<(std::ostream& stream, const xdcm::PublicDictEntry& entry)
		{
			stream << std::setw(8) << std::setfill('0') << std::hex << entry.m_key;
			stream << ", " << entry.VR << ", " << entry.VM << ", " << entry.Name << ", " << entry.Retired << ", " << entry.keyword;
			return stream << std::dec;
		}


	private:
		void guard_ends();

	private:
		unsigned int	m_key;
		char			VR[15];
		char			VM[5];
		char			Name[72];
		char			Retired[8];
		char			keyword[63];
	};

	static  PublicDictEntry empty_sentinel(0x11111111, "OO", "0", "Empty Sentinel", "", "EmptySentinel");
	static  PublicDictEntry deleted_sentinel(0xffffffff, "XX", "F", "Deleted Sentinel", "", "DeletedSentinel");
}

namespace std
{
	template<>
	struct std::hash<xdcm::PublicDictEntry>
	{
		size_t operator()(const xdcm::PublicDictEntry& entry) const noexcept
		{
			size_t h1 = std::hash<unsigned int>{}(entry.key());
			return h1;
		}
	};
}

namespace xdcm
{
	/// <summary>
	/// Singleton Class For DataDictionary
	/// </summary>
	class DataDictionary
	{
	public:

		DataDictionary(const DataDictionary& _other) = delete;
		DataDictionary(DataDictionary&& _other) = delete;
		DataDictionary& operator=(const DataDictionary& _other) = delete;
		DataDictionary& operator=(DataDictionary&& _other) = delete;  

		static DataDictionary* instance();
		const std::vector<PublicDictEntry>& get_table() const;

		const int insert_entry( PublicDictEntry&& entry);
		const PublicDictEntry delete_entry(const PublicDictEntry& entry);


	private:

		DataDictionary();
		// for now
		~DataDictionary() = default;
		static void init_hash_table(std::vector<PublicDictEntry>* _table, std::ifstream* _i_fstream, const std::regex* _regex);	
	private:
		std::vector<PublicDictEntry>	m_table;
		static DataDictionary*			pinstance;
		static std::mutex				m_mutex;
	};

	
}




#endif // !DATA_DICT
