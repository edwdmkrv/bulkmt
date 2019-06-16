#include "bulkmt.hpp"

#include <gtest/gtest.h>

#include <string>
#include <sstream>

using std::literals::string_literals::operator "" s;

struct Data {
	unsigned const bulk;
	std::vector<std::string> const cmds;
	std::vector<std::string> const r;
};

struct Test: testing::Test, testing::WithParamInterface<Data> {
};

std::string input(Data const &data) {
	std::ostringstream t;

	for (auto const &s: data.cmds) {
		t << s << std::endl;
	}

	return t.str();
}

TEST_P(Test, TestBulk) {
	Data const data{GetParam()};

	std::ostringstream o;

	usr::Logger logger{o};
	usr::Filer filer1;
	usr::Filer filer2;

	usr::Interpreter interpreter{data.bulk};

	std::istringstream i{input(data)};

	usr::Reader reader{i};

	reader.subscribe(interpreter);
	interpreter.subscribe(logger);
	interpreter.subscribe(filer1);
	interpreter.subscribe(filer2);
	reader.run();

	std::ostringstream r;

	for (auto const &s: data.r) {
		r << s << std::endl;
	}

	EXPECT_EQ(o.str(), r.str());

	std::filesystem::remove(filer2.path());
	std::filesystem::remove(filer1.path());
}

INSTANTIATE_TEST_CASE_P(SimpleDataset, Test,
	testing::Values(
		Data{
			3,
			{
				"cmd1"s,
				"cmd2"s,
				"cmd3"s,
				"cmd4"s,
				"cmd5"s
			},
			{
				"bulk: cmd1, cmd2, cmd3"s,
				"bulk: cmd4, cmd5"s
			}
		},
		Data{
			3,
			{
				"cmd1"s,
				"cmd2"s,
				"cmd3"s,
				"{"s,
				"cmd4"s,
				"cmd5"s,
				"cmd6"s,
				"cmd7"s,
				"}"s
			},
			{
				"bulk: cmd1, cmd2, cmd3"s,
				"bulk: cmd4, cmd5, cmd6, cmd7"s
			}
		},
		Data{
			3,
			{
				"{"s,
				"cmd1"s,
				"cmd2"s,
				"{"s,
				"cmd3"s,
				"cmd4"s,
				"}"s,
				"cmd5"s,
				"cmd6"s,
				"}"s
			},
			{
				"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6"s
			}
		},
		Data{
			3,
			{
				"cmd1"s,
				"cmd2"s,
				"cmd3"s,
				"{"s,
				"cmd4"s,
				"cmd5"s,
				"cmd6"s,
				"cmd7"s
			},
			{
				"bulk: cmd1, cmd2, cmd3"s
			}
		}
	)
);

