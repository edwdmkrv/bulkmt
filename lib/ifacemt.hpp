#ifndef __IFACEMT_INCLUDED
#define __IFACEMT_INCLUDED

#include <stdexcept>
#include <utility>
#include <memory>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <thread>
#include <vector>

#include <map>

using std::literals::string_literals::operator "" s;

namespace utl {

template <typename X>
struct ISubscriber;

template <typename X>
struct IPublisher {
	virtual void subscribe(ISubscriber<X> &) = 0;
	virtual void bulkupdate(X const &, bool const = false) = 0;
	virtual void bulkflush() = 0;

protected:
	virtual ~IPublisher() = default;
};

template <typename X>
struct IThreadPublisher {
	virtual void subscribe(ISubscriber<X> &) = 0;
	virtual void bulkupdate(X &&, bool const = false) = 0;
	virtual void bulkflush() = 0;

protected:
	virtual ~IThreadPublisher() = default;
};

template <typename X>
struct ISubscriber {
	virtual void update(X const &, bool const = false) = 0;
	virtual void flush() = 0;

protected:
	virtual ~ISubscriber() = default;
};

template <typename X>
class Subscriber: public ISubscriber<X> {
	void flush() override {
	}
};

template <typename X>
class Publisher: IPublisher<X> {
	bool finalized{};
	std::vector<ISubscriber<X> *> subscribers;

protected:
	Publisher() noexcept = default;
	Publisher(Publisher &&) noexcept(noexcept(decltype(subscribers){std::declval<decltype(subscribers)>()})) = default;

	~Publisher() override {
		if (!finalized) {
			bulkflush();
		}
	}

	void bulkupdate(X const &x, bool const b = false) override {
		for (auto &subscriber: subscribers) {
			try {
				subscriber->update(x, b);
			} catch (...) {
			}
		}
	}

	void bulkflush() override {
		for (auto &subscriber: subscribers) {
			try {
				subscriber->flush();
			} catch (...) {
			}
		}
	}

public:
	void subscribe(ISubscriber<X> &subscriber) override {
		subscribers.push_back(&subscriber);
	}
};

template <typename X, typename Mutex = std::shared_mutex>
class ThreadPublisher: IThreadPublisher<X> {
	enum: std::size_t {pool_min = 3};

	template <typename M, std::size_t N = pool_min>
	class MutexPool {
		std::array<M, N> m;

	public:
		class Index {
			std::size_t index;

		public:
			constexpr Index(std::size_t const index = 0) noexcept: index{index} {
			}

			constexpr operator std::size_t() const noexcept {
				return index;
			}

			constexpr Index prev() const noexcept {
				return (index + N - 1) % N;
			}

			constexpr Index next() const noexcept {
				return (index + 1) % N;
			}

			constexpr Index &operator ++() noexcept {
				index = next();
				return *this;
			}
		};

		M &operator [](Index const &index) noexcept {
			return m[index];
		}
	};

	MutexPool<Mutex> mutexpool{};
	typename MutexPool<Mutex>::Index index{};

	bool finished{};
	std::vector<std::thread> threads{};

	std::function<void(ISubscriber<X> &)> task;

	class Joiner {
		std::vector<std::thread> &threads;

	public:
		Joiner(std::vector<std::thread> &threads): threads{threads} {
		}

		~Joiner() {
			for (auto &thread: threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
		}
	} const joiner{threads};

	class Finisher {
		bool &finished;

	public:
		Finisher(bool &finished): finished{finished} {
		}

		~Finisher() {
			finished = true;
		}
	} const finisher{finished};

	std::unique_lock<Mutex> lock{mutexpool[index]};

	void thread(std::promise<void> &promise, ISubscriber<X> &subscriber) {
		auto index{this->index}; // Initialize the per-thread mutex index
		std::shared_lock<Mutex> persistent{mutexpool[index.prev()]}; // Acquire the persistent shared lock

		promise.set_value(); // Signal the creating thread that this thread persistent shared lock has been acquired

		do {
			std::shared_lock<Mutex> temporary{mutexpool[index]}; // Acquire the temporary shared lock

			if (!finished) {
				try {
					task(subscriber);
				} catch (...) {
				}
			}

			persistent.swap(temporary); // Prepare to release the persistent shared lock and make the temporary one persistent
			++index;
		} while (!finished);
	}

	template <typename F, typename... Y>
	void barrier(F &&f, Y &&... y) {
		 // Acquire a new unique lock, thus waiting until all the shared locks in threads are released (the barrier pattern)
		std::unique_lock<Mutex> lock{mutexpool[index.next()]};

		lock.swap(this->lock); // Excahnge unique locks thus releasing the previous lock after leaving the function
		f(std::forward<Y>(y)...);
		++index;
	}

protected:

	void bulkupdate(X &&data, bool const b = false) override {
		barrier([&] {
			task = std::bind(&ISubscriber<X>::update, std::placeholders::_1, std::move(data), b);
		});
	}

	void bulkflush() override {
		barrier([&] {
			task = std::bind(&ISubscriber<X>::flush, std::placeholders::_1);
		});
	}

public:
	void subscribe(ISubscriber<X> &subscriber) override {
		std::promise<void> promise;
		std::future future{promise.get_future()};

		threads.emplace_back(std::thread{&ThreadPublisher::thread, this, std::ref(promise), std::ref(subscriber)});
		future.get(); // Thread function has acquired its persistent shared lock
	}
};

} // namespace utl

#endif
