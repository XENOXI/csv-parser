#pragma once
#include <iostream>
#include <tuple>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>


template <typename Type>
std::istream& operator>>(std::istream& i, const Type& arg) { return i; }

template <class Type>
Type argpars(std::string str)
{	
	Type arg;
	std::stringstream buff;
	buff << str;
	buff >> arg;
	std::string buff2;
	buff2 = str;
	str.clear();
	std::getline(buff, str);
	if (str == buff2) //Nothing changed
		throw std::exception("Not valid argument");
	return arg;
}

template <>
inline std::string argpars(std::string str)
{
	std::string copy = str;
	str.clear();
	return copy;
}





template<class Ch, class Tr, class Tuple, std::size_t N>
struct TuplePrinter {
	static void print(std::basic_ostream<Ch, Tr>& os, const Tuple& t)
	{
		TuplePrinter<Ch, Tr, Tuple, N - 1>::print(os, t);
		os << ", " << std::get<N - 1>(t);
	}
};

template<class Ch, class Tr, class Tuple>
struct TuplePrinter<Ch, Tr, Tuple, 1> {
	static void print(std::basic_ostream<Ch, Tr>& os, const Tuple& t)
	{
		os << std::get<0>(t);
	}
};

template <class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)-> std::basic_ostream<Ch, Tr>&
{
	TuplePrinter<Ch, Tr, decltype(t), sizeof...(Args)>::print(os, t);
	return os;
}

template <class... Args>
class CSVParser
{
private:
	int string_num = 1;
	typedef struct config
	{
		char sep_row;
		char sep_column;
		char sep_sheilding;
	};
	std::ifstream& file;
	bool is_file_ended=false;
	std::tuple<Args...> parser()
	{
		char symb = cnf.sep_column - 1;
		auto get_data = [&]() -> std::string {
			if (symb == cnf.sep_row || file.eof())    // попытка чтения после делителя строк или конца файла
				throw std::exception("Not valid csv file");

			std::string str="";
			symb = cnf.sep_column - 1;
			bool in_case = false;
			bool leave = false;
			while (true)
			{
				file.read(&symb,1);
				if (symb == cnf.sep_sheilding && !leave) {
					in_case = !in_case;
					if (!in_case)
					{
						if (!file.eof())
							file.read(&symb,1);
						break;
					}

					continue;
				}
					

				

				if (file.eof() || (symb == cnf.sep_row && !in_case))
				{
					if (in_case)
						throw std::exception("Not valid csv file"); // не закрытое экранирование
					break;
				}
				if (symb == cnf.sep_column && !in_case)
					break;
				str.push_back(symb);
			}
			return str;
		};

		std::vector<std::string> vec;
		for (int i = 0; i < sizeof...(Args); i++)
		{
			try
			{
				vec.push_back(get_data());
				
			}
			catch (std::exception e)
			{
				std::stringstream ss;
				ss << e.what() << ". Error on line: " << string_num<<", collumn: "<<i+1;
				std::string err = ss.str();
				throw err;
			}
		}
			
		if (symb != cnf.sep_row)
		{
			std::stringstream ss;
			ss << "Not valid csv file. Error on line: " << string_num << ", column: " << sizeof...(Args);
			throw ss.str();
		}
			


		auto flip = [&vec]()->std::string {std::string s = vec.back(); vec.pop_back(); return s; };

		return std::tuple<Args...>(argpars<Args>(flip())...);
	}

public:
	std::tuple<Args...> buff;
	class iterator
	{
		CSVParser* obj_;
	public:
		using value_type = std::tuple<Args...>;
		using reference = const std::tuple<Args...>&;
		using pointer = const std::tuple<Args...>*;
		using iterator_category = std::input_iterator_tag;
		iterator(CSVParser* obj = nullptr) : obj_{ obj } {}
		reference operator*() const { return obj_->buff; }
		iterator& operator++() { increment(); return *this; }
		iterator operator++(int) { increment(); return *this; }
		bool operator==(iterator rhs) const { return obj_ == rhs.obj_; }
		bool operator!=(iterator rhs) const { return !(rhs == *this); }
	protected:
		void increment()
		{
			obj_->next();
			if (!obj_->valid())
				obj_ = nullptr;
		}
	};
	config cnf = {'\n',',','\"'};


	CSVParser(std::ifstream& fle, int code) : file(fle)
	{
		if (!fle.is_open())
			throw std::exception("File not found");
		for (int i = -1; i < code; i++)
			buff=parser();
		string_num += 1;
	}



	iterator begin() { return iterator{ this }; }
	iterator end() { return iterator{}; }
	void next() {
		if (file.eof())
		{
			is_file_ended = true;
			return;
		}
			
		buff = parser();
		string_num += 1;
	}
	bool valid()
	{
		return !is_file_ended;
	}


};


