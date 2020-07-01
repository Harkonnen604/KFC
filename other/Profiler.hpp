#pragma once
#include <chrono>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <atomic>
#include <mutex>
#include <memory>
#include <cassert>
#ifdef _MSC_VER
#include <intrin.h>
#endif

// Usage: put PROFILE_FUNCTION at the beginning of function or PROFILE_SCOPE at the beginning of arbitrary {} scope (affects if line number will be printed)
// Call Profiler::getStats() to grab statistics. Call Profiler::reset() at the most outer rim.
// Supports recursion and threading.
// Currently implemented via rdtsc() on Windows and via std::chrono::high_resolution_clock on other platforms.
//
// Two kinds of durations are tracked - inner and outer.
// Inner duration ticks only when codeblock is on top of the call stack (relatively to other profiled codeblocks)
// Outer duration starts ticking when codeblock enters callstack for the first time and stops when its outermost call leaves the callstack
// Measurable profiler overhead is subtracted from both outer and inner timings for as much as possible
//
// E.g. in the following example
//
// ......[A......[B......[A......[B......B]......A]......B]......A]......[A......[B.......B].......A].......
//   t0      t1      t2      t3      t4      t5      t6      t7      t8      t9      t10      t11      t12
//
// A.inner = t1 + t3 + t5 + t7 + t9 + t11
// A.outer = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t9 + t10 + t11
// B.inner = t2 + t4 + t6 + t10
// B.outer = t2 + t3 + t4 + t5 + t6 + t10

#ifndef PROFILER_ENABLED

#define PROFILE_FUNCTION ((void)0)
#define PROFILE_SCOPE(scope) ((void)0)

#else // PROFILER_ENABLED

// Intermediate macros as required for proper __LINE__ concatenation into variable name
#define _PROFILE_FUNCTION2(function, line) \
  static uint32_t codeBlockIndex ## line = Profiler::registerCodeBlock(function, line); \
  Profiler::ScopeGuard profilerScopeGuard ## line(codeBlockIndex ## line)

#define _PROFILE_SCOPE2(function, line, scope) \
  static uint32_t codeBlockIndex ## line = Profiler::registerCodeBlock(function, line, scope); \
  Profiler::ScopeGuard profilerScopeGuard ## line(codeBlockIndex ## line)

#define _PROFILE_EXPLICIT2(name, line) \
  static uint32_t codeBlockIndex ## line = Profiler::registerCodeBlock(name, line); \
  Profiler::ScopeGuard profilerScopeGuard ## line(codeBlockIndex ## line)

#define _PROFILE_FUNCTION(function, line) _PROFILE_FUNCTION2(function, line)
#define _PROFILE_SCOPE(function, line, scope) _PROFILE_SCOPE2(function, line, scope)
#define _PROFILE_EXPLICIT(name, line) _PROFILE_EXPLICIT2(name, line)

#define PROFILE_FUNCTION _PROFILE_FUNCTION(__FUNCTION__, __LINE__) // shows up as "function" - usual case
#define PROFILE_SCOPE(scope) _PROFILE_SCOPE(__FUNCTION__, __LINE__, scope) // shows up as "function:scope" - useful for profiling part of the function
#define PROFILE_EXPLICIT(name) _PROFILE_EXPLICIT(name, __LINE__) // explicit name - useful for template members and lambdas as they get too long automatic names

class Profiler
{
public:
  struct CodeBlock
  {
    CodeBlock() = default;
    CodeBlock(const char* function, uint32_t line, const char* scope = nullptr) : function(function), line(line), scope(scope) {}

    const char* function;
    uint32_t line;
    const char* scope;
  };

  struct Duration
  {
    Duration() : outer(0), inner(0) {}
    Duration& operator+=(const Duration& other)
    {
      outer += other.outer;
      inner += other.inner;
      return *this;
    }
    Duration& operator/=(uint32_t divisor)
    {
      assert(divisor);
      outer /= divisor;
      inner /= divisor;
      return *this;
    }
    Duration& updateMax(const Duration& other)
    {
      outer = std::max(outer, other.outer);
      inner = std::max(inner, other.inner);
      return *this;
    }

    uint64_t outer, inner; // ticks within profiler, microseconds within stats
  };

  static constexpr uint32_t MAX_CODEBLOCKS = 4096;
  using Durations = std::array<Duration, MAX_CODEBLOCKS>;

  static uint32_t registerCodeBlock(const char* function, uint32_t line, const char* scope = nullptr);

  // Tick is passed as argument to ensure that function call goes into profiler overhead
  static void enterCodeBlock(uint32_t codeBlockIndex, uint64_t tick = Profiler::getTick());
  static void leaveCodeBlock(uint64_t tick = Profiler::getTick());

  class ScopeGuard
  {
  public:
    ScopeGuard(uint32_t codeBlockIndex)
    {
      if ((wasEnabled = enabled))
        enterCodeBlock(codeBlockIndex);
    }
    ~ScopeGuard()
    {
      if (wasEnabled)
        leaveCodeBlock();
    }
  private:
    bool wasEnabled;
  };

  struct StatsEntry
  {
    StatsEntry& operator+=(const StatsEntry& other)
    {
      threadMaxDuration += other.threadMaxDuration;
      totalDuration += other.totalDuration;
      return *this;
    }
    StatsEntry& operator/=(uint32_t divisor)
    {
      threadMaxDuration /= divisor;
      totalDuration /= divisor;
      return *this;
    }
    StatsEntry& updateMax(const StatsEntry& other)
    {
      threadMaxDuration.updateMax(other.threadMaxDuration);
      totalDuration.updateMax(other.totalDuration);
      return *this;
    }

    Duration threadMaxDuration;
    Duration totalDuration;
  };

  class Stats : public std::map<const CodeBlock*, StatsEntry>
  {
  public:
    Stats& operator+=(const Stats& other)
    {
      for (auto& record : other)
        (*this)[record.first] += record.second;
      return *this;
    }
    Stats& operator/=(uint32_t divisor)
    {
      assert(divisor);
      for (auto& record : *this)
        record.second /= divisor;
      return *this;
    }
    Stats& updateMax(const Stats& other)
    {
      for (auto& record : other)
        (*this)[record.first].updateMax(record.second);
      return *this;
    }
  };

  static Stats getStats();
  static void resetStats();

  static bool enabled;

private:
  static uint64_t getTick()
  {
#ifdef _MSC_VER
    return __rdtsc();
#else
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
  }

  struct CallStackEntry
  {
    CallStackEntry(uint32_t codeBlockIndex) :
      codeBlockIndex(codeBlockIndex), outerStartTick(0), innerStartTick(0), overhead(0) {}

    uint32_t codeBlockIndex;
    uint64_t outerStartTick, innerStartTick, overhead;
  };

  static std::array<CodeBlock, MAX_CODEBLOCKS> codeBlocks;
  static std::atomic<uint32_t> nCodeBlocks;
  static thread_local std::vector<CallStackEntry> callStack;
  static thread_local std::array<uint32_t, MAX_CODEBLOCKS> callDepth;
  static thread_local Durations durations;
  static std::vector<Durations*> threadsDurations; // durations gathered from all threads
  static std::mutex threadsDurationsMutex;
  static std::chrono::high_resolution_clock::time_point timeOfLastReset;
  static uint64_t tickOfLastReset;

  class ThreadRegisterer
  {
  public:
    ThreadRegisterer();
    ~ThreadRegisterer();

  private:
    // Safeguard if some compiler calls thread_local constructor/destructor on main thread when other statics are dead
    static std::atomic_bool mainThreadConstructed;
    struct MainThreadGuard
    {
      MainThreadGuard() { mainThreadConstructed = true; }
      ~MainThreadGuard() { mainThreadConstructed = false; }
    };
    static MainThreadGuard mainThreadGuard;
  };
  static thread_local ThreadRegisterer threadRegisterer; // registers durations inside threadsDurations
};

#endif // PROFILER_ENABLED
