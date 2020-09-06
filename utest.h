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

#define UASSERT(condition) UTest::Assert(STRIFY(condition), PRETTY_FUNCTION, __FILE__, __LINE__, cond)

//holds a result information
struct ResultPair
{
	const char* name;
	const char* funcName;
	const char* file;
	int line;
};

struct Results
{
    using T = ResultPair;
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


//We have to do this to be able to make it work with C++ 14 and below
template<typename T = void>
struct Count
{
	static int count_;
};
template<typename T>
int Count<T>::count_{};

template<typename T = void>
struct StoponFail
{
	static bool stopOnFail_;
};
template<typename T>
bool StoponFail<T>::stopOnFail_ = false;

template<typename T>
struct TestResults
{
	static Results failedResults_;
};
template<typename T>
Results TestResults<T>::failedResults_;

//The main test class
class UTest : private Count<void>, private StoponFail<void>, private TestResults<ResultPair>
{
public:
    UTest() = default;

	/**
	 * @brief stop as soon as a test fails, rest of the tests are ignored
	 */
	static void stopOnFirstFailure(bool value = true)
	{
		stopOnFail_ = value;
	}

	/**
	 * @brief This doesn't assert anything technically. It just adds the result to a list.
	 * 	Don't use this function directly! Use the macro UASSERT instead
	 */
    static void Assert(const char* name, const char* func, const char* file, int line, bool check)
    {
        ++count_;
        if (!check) {
            failedResults_.add(ResultPair{name, func, file, line});
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

    static void report()
    {
        fprintf(stderr, "%d Tests run.\n", count_);
        bool allPassed = failedResults_.size() == 0;
        for (const auto& rp : failedResults_) {
            PRINT_RED("\n[FAIL]");
            fprintf(stderr, ": %s\n", rp.name);
            const char* funcName = rp.funcName;
            const int line = rp.line;
			fprintf(stderr, "\tIn file \"%s\" ", rp.file);
            fprintf(stderr, "at line '%d' in \"%s\"", line, funcName);
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
};

#endif
