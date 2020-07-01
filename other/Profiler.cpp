#include "Profiler.hpp"
#include <algorithm>

#ifdef PROFILER_ENABLED

bool Profiler::enabled = false;
std::array<Profiler::CodeBlock, Profiler::MAX_CODEBLOCKS> Profiler::codeBlocks;
std::atomic<uint32_t> Profiler::nCodeBlocks(0);
std::atomic_bool Profiler::ThreadRegisterer::mainThreadConstructed = false;
Profiler::ThreadRegisterer::MainThreadGuard Profiler::ThreadRegisterer::mainThreadGuard;
std::vector<Profiler::Durations*> Profiler::threadsDurations;
std::mutex Profiler::threadsDurationsMutex;
std::chrono::high_resolution_clock::time_point Profiler::timeOfLastReset = std::chrono::high_resolution_clock::now();
uint64_t Profiler::tickOfLastReset = Profiler::getTick();
thread_local std::vector<Profiler::CallStackEntry> Profiler::callStack;
thread_local std::array<uint32_t, Profiler::MAX_CODEBLOCKS> Profiler::callDepth = {};
thread_local Profiler::Durations Profiler::durations;
thread_local Profiler::ThreadRegisterer Profiler::threadRegisterer;

Profiler::ThreadRegisterer::ThreadRegisterer()
{
  assert(mainThreadConstructed);
  std::lock_guard<std::mutex> lock(threadsDurationsMutex);
  assert(std::find(threadsDurations.begin(), threadsDurations.end(), &durations) == threadsDurations.end());
  threadsDurations.push_back(&durations);
}

Profiler::ThreadRegisterer::~ThreadRegisterer()
{
  // On MSVC std::async is non-conformant (to a good point) in that it uses a thread pool to manage dispatched tasks instead of
  //  always creating a new thread (and paying the cost of creating the new thread).
  // However this causes thread_local variables to not be cleaned up before main() exits in some cases so it needs to be checked here.
  // More information: https://stackoverflow.com/questions/50897768/in-visual-studio-thread-local-variables-destructor-not-called-when-used-with

  if (!mainThreadConstructed)
    return;

  std::lock_guard<std::mutex> lock(threadsDurationsMutex);
  auto i = std::find(threadsDurations.begin(), threadsDurations.end(), &durations);
  assert(i != threadsDurations.end());
  threadsDurations.erase(i);
}

uint32_t Profiler::registerCodeBlock(const char* function, uint32_t line, const char* scope)
{
  uint32_t codeBlockIndex = nCodeBlocks++;
  assert(codeBlockIndex < MAX_CODEBLOCKS);
  codeBlocks[codeBlockIndex] = CodeBlock(function, line, scope);
  return codeBlockIndex;
}

void Profiler::enterCodeBlock(uint32_t codeBlockIndex, uint64_t tick)
{
  assert(codeBlockIndex < nCodeBlocks);
  if (!callStack.empty())
  {
    CallStackEntry& topEntry = callStack.back();
    durations[topEntry.codeBlockIndex].inner += tick - topEntry.innerStartTick;
  }
  callStack.emplace_back(codeBlockIndex);
  callDepth[codeBlockIndex]++;
  {
    CallStackEntry& newTopEntry = callStack.back();
    uint64_t newTick = getTick();
    newTopEntry.outerStartTick = tick;
    newTopEntry.innerStartTick = newTick;
    newTopEntry.overhead = newTick - tick;
  }
}

void Profiler::leaveCodeBlock(uint64_t tick)
{
  assert(!callStack.empty());
  CallStackEntry& topEntry = callStack.back();
  durations[topEntry.codeBlockIndex].inner += tick - topEntry.innerStartTick;
  assert(callDepth[topEntry.codeBlockIndex] > 0);
  if (--callDepth[topEntry.codeBlockIndex] == 0)
    durations[topEntry.codeBlockIndex].outer += tick - topEntry.outerStartTick - topEntry.overhead;
  uint64_t innerOverhead = topEntry.overhead;
  callStack.pop_back();
  if (!callStack.empty())
  {
    CallStackEntry& newTopEntry = callStack.back();
    uint64_t newTick = getTick();
    newTopEntry.innerStartTick = newTick;
    newTopEntry.overhead += innerOverhead + newTick - tick;
  }
}

Profiler::Stats Profiler::getStats()
{
  std::lock_guard<std::mutex> lock(threadsDurationsMutex);
  Stats stats;
  for (const Durations* durations : threadsDurations)
    for (uint32_t i = 0; i < nCodeBlocks; i++)
    {
      stats[&codeBlocks[i]].threadMaxDuration.updateMax((*durations)[i]);
      stats[&codeBlocks[i]].totalDuration += (*durations)[i];
    }
  uint32_t microseconds = Math::max((uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeOfLastReset).count(), 1u);
  uint32_t ticksPerMicrosecond = Math::max((uint32_t)(getTick() - tickOfLastReset) / microseconds, 1u);
  for (auto& entry : stats)
    entry.second /= ticksPerMicrosecond;
  return stats;
}

void Profiler::resetStats()
{
  for (Durations* durations : threadsDurations)
    for (uint32_t i = 0; i < nCodeBlocks; i++)
      (*durations)[i] = Duration();
  timeOfLastReset = std::chrono::high_resolution_clock::now();
  tickOfLastReset = getTick();
}

#endif // PROFILER_ENABLED
