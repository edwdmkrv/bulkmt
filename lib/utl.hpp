#ifndef __LIB_INCLUDED
#define __LIB_INCLUDED

#include <string>
#include <map>

#include <version.hpp>

namespace utl {

class Multiple {
	std::map<std::string, std::pair<std::string, std::string>> const words;

public:
	Multiple(std::initializer_list<std::pair<std::string const, std::pair<std::string, std::string>>> words): words{words} {
	}

	std::string const get(std::string const &word, unsigned const n) const {
		auto const &item{words.at(word)};

		auto const ones{n % 10};
		bool const exception{n % 100 - n % 10 == 10};
		bool const first{!exception && ones == 1};
		bool const second{!exception && ones > 1 && ones < 5};

		return std::to_string(n) + ' ' + (first ? word : second ? item.first : item.second);
	}
};

} // namespace utl

#endif
