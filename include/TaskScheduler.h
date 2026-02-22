#pragma once
// esp32emu — TaskScheduler cooperative multitasking mock

#include <cstdint>
#include <cstdio>
#include <functional>
#include <vector>
#include <algorithm>

#define TASK_MILLISECOND  1
#define TASK_SECOND       1000
#define TASK_MINUTE       60000
#define TASK_FOREVER      -1
#define TASK_ONCE         1
#define TASK_IMMEDIATE    0

class Task;
class Scheduler;

class Task {
public:
    using Callback = std::function<void()>;

    Task() = default;
    Task(unsigned long interval, long iterations, Callback cb, Scheduler* sched = nullptr,
         bool enabled = false, Callback onEnable = nullptr, Callback onDisable = nullptr)
        : interval_(interval), iterations_(iterations), max_iterations_(iterations),
          callback_(cb), on_enable_(onEnable), on_disable_(onDisable) {
        (void)sched;
        if (enabled) enable();
    }

    void set(unsigned long interval, long iterations, Callback cb) {
        interval_ = interval;
        iterations_ = iterations;
        max_iterations_ = iterations;
        callback_ = cb;
    }

    void setInterval(unsigned long ms) { interval_ = ms; }
    unsigned long getInterval() const { return interval_; }
    void setIterations(long n) { iterations_ = n; max_iterations_ = n; }
    long getIterations() const { return iterations_; }

    void enable()  { enabled_ = true; run_count_ = 0; iterations_ = max_iterations_; if (on_enable_) on_enable_(); }
    void disable() { enabled_ = false; if (on_disable_) on_disable_(); }
    void restart() { enable(); }
    bool isEnabled() const { return enabled_; }

    void enableDelayed(unsigned long ms = 0) { (void)ms; enable(); }

    bool isFirstIteration() const { return run_count_ == 1; }
    bool isLastIteration() const { return iterations_ == 0; }
    long getRunCounter() const { return run_count_; }

    void setCallback(Callback cb)    { callback_ = cb; }
    void setOnEnable(Callback cb)    { on_enable_ = cb; }
    void setOnDisable(Callback cb)   { on_disable_ = cb; }

    // Called by scheduler
    bool _execute() {
        if (!enabled_ || !callback_) return false;
        if (iterations_ == 0) { disable(); return false; }
        run_count_++;
        callback_();
        if (iterations_ > 0) iterations_--;
        if (iterations_ == 0) { disable(); return false; }
        return true;
    }

private:
    unsigned long interval_ = 0;
    long iterations_ = -1;
    long max_iterations_ = -1;
    long run_count_ = 0;
    bool enabled_ = false;
    Callback callback_;
    Callback on_enable_;
    Callback on_disable_;
};

class Scheduler {
public:
    Scheduler() = default;

    void init() { fprintf(stderr, "[esp32emu] Scheduler: initialized\n"); }

    void addTask(Task& t) {
        tasks_.push_back(&t);
    }

    void deleteTask(Task& t) {
        tasks_.erase(std::remove(tasks_.begin(), tasks_.end(), &t), tasks_.end());
    }

    bool execute() {
        bool any = false;
        for (auto* t : tasks_) {
            if (t->isEnabled()) {
                if (t->_execute()) any = true;
            }
        }
        return any;
    }

    size_t size() const { return tasks_.size(); }

    // Test helper
    std::vector<Task*>& _getTasks() { return tasks_; }

private:
    std::vector<Task*> tasks_;
};
