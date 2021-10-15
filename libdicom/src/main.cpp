#include <iostream>


#include "./dataelem/RawDataElement.h"
#include "./tag/tag.h"
#include "./dict/data_dict.h"
#include "./util/timer.h"


int main()
{
	
	
	{
		TIME_SCOPE("overall");
		auto& table = xdcm::DataDictionary::instance()->get_table();

		
		xdcm::DataDictionary::instance()->delete_entry(table[2]);
		auto& f = xdcm::DataDictionary::instance()->delete_entry(table[2]);
		int index = xdcm::DataDictionary::instance()->insert_entry(xdcm::PublicDictEntry(0x22222222, "SQ", "1", "DUMMY ENTRY", "Retired", "DUMMYENTRY"));
		int index_2 = xdcm::DataDictionary::instance()->insert_entry(xdcm::PublicDictEntry(0x33333333, "SQ", "1", "DUMMY ENTRY", "Retired", "DUMMYENTRY"));

		std::cout << table[2] << '\n';
		std::cout << table[index] << '\n';
		std::cout << table[index_2] << '\n';

		int counter = 0;
		for (auto& entry : table)
		{
			//std::cout << entry << '\n';
			if (entry.key() != xdcm::empty_sentinel.key() && entry.key() != xdcm::deleted_sentinel.key())
			{
				//std::cout << entry << '\n';
				counter++;
			}
		}

		std::cout << counter << '\n';
		std::cout << std::boolalpha << (counter == 4839) << '\n';


	}

	

	std::cin.get();
	return 0;	
}