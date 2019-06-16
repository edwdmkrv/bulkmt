#ifndef __BULKMT_INCLUDED
#define __BULKMT_INCLUDED

#include <cstdlib>
#include <ctime>

#include <chrono>
#include <iterator>
#include <memory>
#include <thread>
#include <mutex>

#if __has_include(<filesystem>)

#include <filesystem>

#elif __has_include(<experimental/filesystem>)

#include <experimental/filesystem>

namespace std {

namespace filesystem = experimental::filesystem;

}

#else

#error "Cannot find <filesystem> header for inclusion"

#endif

#include <sstream>
#include <fstream>
#include <iostream>

#include "ifacemt.hpp"

namespace usr {

using Cmd = std::string;
using Bulk = std::vector<Cmd>;

namespace internal {

class Writer: public utl::Subscriber<Bulk> {
	unsigned blks{};
	unsigned cmds{};

	std::ostream &o;

	void update(Bulk const &v, bool const b) override final {
		auto const &cend{std::cend(v)};
		auto cit{std::cbegin(v)};

		if (cit != cend) {
			o << "bulk: ";

			for (auto const &clast{std::prev(cend)}; cit != clast; ++cit) {
				o << *cit << ", ";
			}

			o << *cit << std::endl;
			cmds += v.size();
		}

		if (b) {
			blks++;
		}
	}

protected:
	Writer(std::ostream &o = std::cout): o{o} {
		o.exceptions(std::ostream::badbit | std::ostream::failbit | std::ostream::eofbit);
	}

public:
	auto getblks() const noexcept {
		return blks;
	}

	auto getcmds() const noexcept {
		return cmds;
	}
};

class PreFiler {
	std::filesystem::path const name;
	std::ofstream file;

	static inline int rand() noexcept {
		static std::once_flag flag;

		call_once(flag, std::srand, std::time(nullptr));
		return std::rand();
	}

protected:
	PreFiler()
	: name{std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) + '-' + std::to_string(rand()) + ".log"}
	, file{name} {
	}

	void flush() {
		file.flush();
	}

	std::filesystem::path const &path() const noexcept {
		return name;
	}

	std::ostream &ostream() noexcept {
		return file;
	}
};

} // namespace internal

struct Logger final: internal::Writer {
	Logger(std::ostream &o = std::cout): Writer{o} {
	}
};

class Filer final: internal::PreFiler, public internal::Writer {
	void flush() override {
		PreFiler::flush();
	}

public:
	Filer(): Writer{ostream()} {
	}

	using PreFiler::path;
};


class Interpreter final: public utl::ThreadPublisher<Bulk>, public utl::Subscriber<Cmd> {
	unsigned strs{};
	unsigned cmds{};
	unsigned blks{};

	unsigned cmdx{};

	unsigned nesting{};

	class BulkWrapper {
		unsigned const bulksz;
		Bulk data;

	public:
		BulkWrapper(unsigned const bulksize): bulksz{bulksize} {
			data.reserve(bulksz);
		}

		Bulk *operator ->() noexcept {
			return &data;
		}

		operator Bulk &&() noexcept {
			return std::move(data);
		}

		unsigned bulksize() const noexcept {
			return bulksz;
		}
	} data;

	void update(Cmd const &s, bool const) override final {
		bool blk{};

		strs++;

		if (s == "{") {
			if (nesting++ || !data->size()) {
				return;
			}
		} else {
			if (s == "}") {
				if (!nesting || --nesting) {
					return;
				}

				blk = true;
			} else {
				data->push_back(s);
				cmdx++;

				if (nesting || data->size() != data.bulksize()) {
					return;
				}
			}
		}

		if (blk) {
			blks++;
		}

		cmds += cmdx;
		cmdx = 0;

		bulkupdate(data, blk);
	}

	void flush() noexcept override final {
		if (!nesting && !data->empty()) {
			cmds += data->size();
			bulkupdate(data, false);
		}

		bulkflush();
	}

public:
	Interpreter(unsigned const bulksize): data{bulksize} {
	}

	~Interpreter() override final {
		flush();
	}

	auto getstrs() const noexcept {
		return strs;
	}

	auto getcmds() const noexcept {
		return cmds;
	}

	auto getblks() const noexcept {
		return blks;
	}
};

class Reader final: public utl::Publisher<std::string> {
	std::istream &istream;

public:
	Reader(std::istream &istream = std::cin): istream{istream} {
	}

	void run() {
		std::string line;

		while (std::getline(istream, line)) {
			bulkupdate(line);
		}

		bulkflush();
	}
};

} // namespace usr

#endif
