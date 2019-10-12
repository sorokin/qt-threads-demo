#pragma once

#include <QObject>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

struct background_thread : QObject
{
private:
    Q_OBJECT
public:
    struct result
    {
        result();
    
        std::vector<uint64_t> factors;
        bool incomplete;
    };

    background_thread();

    ~background_thread();

    void set_number_to_factor(uint64_t n);
    result get_result() const;

signals:
    void result_changed();

private:
    void factor(uint64_t a);
    void queue_callback();
    void callback();

private:
    mutable std::mutex m;
    uint64_t number_to_factor;
    bool quit;
    bool callback_queued;
    std::atomic<bool> cancel;
    std::condition_variable has_work_cv;
    result current_result;
    std::thread thread;
};
