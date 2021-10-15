#include "data_dict.h"



namespace xdcm
{
	PublicDictEntry::PublicDictEntry(unsigned int _key, const char* _vr, const char* _vm, const char* _name, const char* _retired, const char* _keyword)
	{
		guard_ends();

		m_key = _key;
		std::memcpy(VR, _vr, 14);
		std::memcpy(VM, _vm, 4);
		std::memcpy(Name, _name, 71);
		std::memcpy(Retired, _retired, 7);
		std::memcpy(keyword, _keyword, 62);
	}
	
	const unsigned int& PublicDictEntry::key() const
	{
		return m_key;
	}

	void PublicDictEntry::guard_ends()
	{
		VR[14]		= '\0';
		VM[4]		= '\0';
		Name[71]	= '\0';
		Retired[7]	= '\0';
		keyword[62] = '\0';
	}

	DataDictionary* DataDictionary::instance()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (pinstance == nullptr)
			pinstance = new DataDictionary;
		return pinstance;
	}

	const std::vector<PublicDictEntry>& DataDictionary::get_table() const
	{
		return m_table;
	}


	DataDictionary::DataDictionary()
	{
		m_table.reserve(5171);

		for (int i = 0; i < m_table.capacity(); i++)
			m_table.emplace_back(empty_sentinel);

		std::regex _regex;
#ifdef _DEBUG
		try
		{
			_regex = std::regex("(\\d{8}|[0-9a-f]{8}),\\s*([A-Za-z\\s]{2,14}),\\s*(\\d-*\\d*n*),\\s*([0-9A-Za-z\\s'(&)\\/-]{0,71}),\\s*(\\s|Retired),\\s{0,2}([0-9A-Za-z-]*)",
				std::regex::ECMAScript | std::regex::optimize);
		}
		catch (std::regex_error& e)
		{
			std::cout << e.what() << '\n';
			_regex = std::regex(".*");
		}
#else
		_regex = std::regex("(\\d{8}|[0-9a-f]{8}),\\s*([A-Za-z\\s]{2,14}),\\s*(\\d-*\\d*n*),\\s*([0-9A-Za-z\\s'(&)\\/-]{0,71}),\\s*(\\s|Retired),\\s{0,2}([0-9A-Za-z-]*)", // final group match is a little hackish not proud of it
			std::regex::ECMAScript | std::regex::optimize);
#endif
		std::ifstream* i_fstream = new std::ifstream[4];
		i_fstream[0].open("F:\\Programming\\dev\\dicom_lib\\libdicom\\libdicom\\data\\dict\\dicom.dict");
		i_fstream[1].open("F:\\Programming\\dev\\dicom_lib\\libdicom\\libdicom\\data\\dict\\dicom-2.dict");
		i_fstream[2].open("F:\\Programming\\dev\\dicom_lib\\libdicom\\libdicom\\data\\dict\\dicom-3.dict");
		i_fstream[3].open("F:\\Programming\\dev\\dicom_lib\\libdicom\\libdicom\\data\\dict\\dicom-4.dict");

		std::thread t1(init_hash_table, &m_table, &i_fstream[0], &_regex);
		std::thread t2(init_hash_table, &m_table, &i_fstream[1], &_regex);
		std::thread t3(init_hash_table, &m_table, &i_fstream[2], &_regex);
		std::thread t4(init_hash_table, &m_table, &i_fstream[3], &_regex);

		t1.join();
		t2.join();
		t3.join();
		t4.join();

		// close file streams 
		for (int i = 0; i < 4; i++)
			i_fstream[i].close();

		delete[] i_fstream;

	}



	void DataDictionary::init_hash_table(std::vector<PublicDictEntry>* _table, std::ifstream* _i_fstream, const std::regex* _regex)
	{
		if (_i_fstream->is_open())
		{
			std::string line;

			while (std::getline(*_i_fstream, line))
			{
				if (std::regex_match(line, std::regex("^\\s*(//).*"))) // ignore comments
				{
					continue;
				}

				// match groups are TAG, VR, VM, NAME, RETIRED, KEYWORD
				std::sregex_iterator begin(line.begin(), line.end(), *_regex);
				std::sregex_iterator end;

				while (begin != end)
				{
					unsigned int key = (unsigned int)std::stoul((*begin)[1], nullptr, 16);
					std::string VR((*begin)[2]);
					std::string VM((*begin)[3]);
					std::string Name((*begin)[4]);
					std::string RETIRED((*begin)[5]);
					std::string keyword((*begin)[6]);

					xdcm::PublicDictEntry tmp(key, VR.c_str(), VM.c_str(), Name.c_str(), RETIRED.c_str(), keyword.c_str());

					// insert(tmp);

					for (int i = 0; i < _table->capacity(); i++)
					{
						unsigned int try_index = (std::hash<xdcm::PublicDictEntry>{}(tmp)+(i * std::hash<xdcm::PublicDictEntry>{}(tmp))) % _table->capacity();
						if ((*_table)[try_index].key() == empty_sentinel.key() || (*_table)[try_index].key() == deleted_sentinel.key())
						{
							(*_table)[try_index] = std::move(tmp);
							break;
						}
						else
						{
							continue;
						}
					}
					++begin;
				}
			}
		}
		else if (_i_fstream->bad())
		{
			std::cout << "bad boy!" << '\n';
		}
	}

	/// <summary>
	/// insert into the hash table
	/// </summary>
	/// <param name="entry">the entry to insert</param>
	/// <returns>index of the entry in the hash table or -1 if no empty space is found</returns>
	const int DataDictionary::insert_entry(PublicDictEntry&& entry)
	{
		
		for (int i = 0; i < m_table.capacity(); i++)
		{
			unsigned int try_index = (std::hash<xdcm::PublicDictEntry>{}(entry)+(i * std::hash<xdcm::PublicDictEntry>{}(entry))) % m_table.capacity();
			if (m_table[try_index].key() == empty_sentinel.key() || m_table[try_index].key() == deleted_sentinel.key())
			{
				m_table[try_index] = std::move(entry);
				return try_index;
			}
			else
			{
				continue;
			}

		}

		return -1;

	}

	const PublicDictEntry DataDictionary::delete_entry(const PublicDictEntry& entry)
	{

		for (int i = 0; i < m_table.capacity(); i++)
		{
			unsigned int try_index = (std::hash<xdcm::PublicDictEntry>{}(entry)+(i * std::hash<xdcm::PublicDictEntry>{}(entry))) % m_table.capacity();
			if (m_table[try_index].key() == empty_sentinel.key() || m_table[try_index].key() == deleted_sentinel.key())
			{
				return empty_sentinel;

			}
			else if (m_table[try_index].key() == entry.key())
			{
				PublicDictEntry tmp = m_table[try_index];
				m_table[try_index] = deleted_sentinel;
				return tmp;
			}

		}

		return empty_sentinel;
	}


	std::mutex		DataDictionary::m_mutex;
	DataDictionary* DataDictionary::pinstance{ nullptr };
}
