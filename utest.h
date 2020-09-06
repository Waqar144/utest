#ifndef __LIB_U_TEST__
#define __LIB_U_TEST__

#include <cstdio>
#include <cstdlib>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET_COLOR "\x1b[0m"

#define PRINT_RED(...) fprintf(stderr, RED __VA_ARGS__ RESET_COLOR)
#define PRINT_GREEN(...) fprintf(stderr, GREEN __VA_ARGS__ RESET_COLOR)

#define STRIFY_IMPL(S) #S
#define STRIFY(S) STRIFY_IMPL(S)

#if defined(__GNUC__) || defined(__clang__)
	#define PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	#define PRETTY_FUNCTION __FUNCSIG__
#endif

#define UASSERT(cond) UTest::Assert(STRIFY(cond), PRETTY_FUNCTION, __LINE__, cond)

template<typename T>
struct Results
{
    Results() : results_{nullptr}, capacity_{1}, size_{0} {}

    T* begin() { return &results_[0]; }
    T* end() { return &results_[size_]; }
    inline size_t size() { return size_; }

    void add(const T& item)
    {
        if (capacity_ == 1 || size_ == capacity_)
            grow();
        results_[size_] = item;
        size_++;
    }

    ~Results() { free(results_); }
private:
    void grow()
    {
		results_ = static_cast<T*>(
				std::realloc(static_cast<void*>(results_),
							sizeof(T) * capacity_ * 2)
				);
		if (!results_) {
			fprintf(stderr, RED "Allocation failed! Exiting...\n");
			exit(EXIT_FAILURE);
		}
		capacity_ *= 2;
    }

    T* results_;
    size_t capacity_;
    size_t size_;
};

class UTest
{
public:
    UTest() = default;

	static void stopOnFirstFailure(bool value = true)
	{
		stopOnFail_ = value;
	}

    static void Assert(const char* name, const char* func, int line, bool check)
    {
        ++count_;
        if (!check) {
            failedResults_.add(ResultPair{name, func, line});
			if (stopOnFail_) {
				report();
				exit(EXIT_FAILURE);
			}
		}
    }

	~UTest()
	{
		report();
	}

private:
	struct ResultPair
	{
		const char* name;
        const char* funcName;
        int line;
	};

    static void report()
    {
        fprintf(stderr, "%d Tests run.\n", count_);
        bool allPassed = failedResults_.size() == 0;
        for (const auto& rp : failedResults_) {
            PRINT_RED("[FAIL]");
            fprintf(stderr, ": %s\n", rp.name);
            const char* funcName = rp.funcName;
            const int line = rp.line;
            fprintf(stderr, "\tat Line '%d' in \"%s\"", line, funcName);
        }
        if (!allPassed) {
            fprintf(stderr, "\n------------------------------\n");
            PRINT_RED("Some tests failed!\n");
            fprintf(stderr, "------------------------------\n");
        } else {
            fprintf(stderr, "\n------------------------------\n");
            PRINT_GREEN("All tests passed\n");
            fprintf(stderr, "------------------------------\n");
        }
    }

    inline static Results<ResultPair> failedResults_{};
    inline static int count_ = 0;
	inline static bool stopOnFail_ = false;
};

#endif
