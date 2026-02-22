// esp32emu — TaskScheduler test
#include "TaskScheduler.h"
#include <cassert>
#include <cstdio>

int main() {
    Scheduler scheduler;
    scheduler.init();

    // Basic task
    int count = 0;
    Task t1(100, TASK_FOREVER, [&]() { count++; });
    scheduler.addTask(t1);
    assert(scheduler.size() == 1);

    // Not enabled yet
    scheduler.execute();
    assert(count == 0);

    // Enable
    t1.enable();
    assert(t1.isEnabled());
    scheduler.execute();
    assert(count == 1);
    assert(t1.isFirstIteration());
    scheduler.execute();
    assert(count == 2);
    assert(!t1.isFirstIteration());

    // Limited iterations
    int count2 = 0;
    Task t2(50, 3, [&]() { count2++; });
    scheduler.addTask(t2);
    t2.enable();
    assert(t2.getIterations() == 3);

    scheduler.execute(); // both run
    assert(count2 == 1);
    scheduler.execute();
    assert(count2 == 2);
    scheduler.execute();
    assert(count2 == 3);
    assert(t2.isLastIteration());
    assert(!t2.isEnabled()); // auto-disabled

    scheduler.execute();
    assert(count2 == 3); // no more

    // Disable/restart
    t1.disable();
    assert(!t1.isEnabled());
    int before = count;
    scheduler.execute();
    assert(count == before);

    t1.restart();
    assert(t1.isEnabled());

    // One-shot
    int once_count = 0;
    Task t3(0, TASK_ONCE, [&]() { once_count++; });
    scheduler.addTask(t3);
    t3.enable();
    scheduler.execute();
    assert(once_count == 1);
    assert(!t3.isEnabled());

    // Interval and config
    t1.setInterval(200);
    assert(t1.getInterval() == 200);
    t1.setIterations(10);
    assert(t1.getIterations() == 10);

    // On enable/disable callbacks
    int en_count = 0, dis_count = 0;
    Task t4;
    t4.set(100, TASK_FOREVER, [&]() {});
    t4.setOnEnable([&]() { en_count++; });
    t4.setOnDisable([&]() { dis_count++; });
    t4.enable();
    assert(en_count == 1);
    t4.disable();
    assert(dis_count == 1);

    // Delete task
    scheduler.deleteTask(t3);
    assert(scheduler.size() == 2);

    // Run counter
    int rc = 0;
    Task t5(10, 5, [&]() { rc++; });
    scheduler.addTask(t5);
    t5.enable();
    scheduler.execute();
    assert(t5.getRunCounter() == 1);

    // enableDelayed
    Task t6(10, TASK_FOREVER, [&]() {});
    t6.enableDelayed(100);
    assert(t6.isEnabled());

    printf("test_taskscheduler: all assertions passed\n");
    return 0;
}
