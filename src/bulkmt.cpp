#include <cstdlib>

#include <iostream>

#include "utl.hpp"
#include "bulkmt.hpp"

using std::literals::string_literals::operator "" s;

#include <chrono>
#include <thread>

int main() try {
	enum: unsigned {bulk = 3};

	usr::Logger logger;
	usr::Filer filer1;
	usr::Filer filer2;
	usr::Interpreter interpreter{bulk};
	usr::Reader reader;

	reader.subscribe(interpreter);
	interpreter.subscribe(logger);
	interpreter.subscribe(filer1);
	interpreter.subscribe(filer2);
	reader.run();

	utl::Multiple const multiple{
		{"строка"s,  {"строки"s,   "строк"s}},
		{"команда"s, {"команды"s, "команд"s}},
		{"блок"s,    {"блока"s,   "блоков"s}}
	};

	std::cout <<
	std::endl <<
		"main поток - " << multiple.get("строка", interpreter.getstrs()) <<
		", " << multiple.get("команда", interpreter.getcmds()) <<
		", " << multiple.get("блок", interpreter.getblks()) <<
	std::endl;
	std::cout <<
		"log поток - " << multiple.get("блок", logger.getblks()) <<
		", " << multiple.get("команда", logger.getcmds()) <<
	std::endl;
	std::cout <<
		"file1 поток - " << multiple.get("блок", filer1.getblks()) <<
		", " << multiple.get("команда", filer1.getcmds()) <<
	std::endl;
	std::cout <<
		"file2 поток - " << multiple.get("блок", filer2.getblks()) <<
		", " << multiple.get("команда", filer2.getcmds()) <<
	std::endl <<
	std::endl;

	return EXIT_SUCCESS;
} catch (std::exception const &e) {
	std::cerr << "Error: " << e.what() << std::endl;
	return EXIT_FAILURE;
} catch (...) {
	std::cerr << "Error: " << "unidentified exception" << std::endl;
	return EXIT_FAILURE;
}
