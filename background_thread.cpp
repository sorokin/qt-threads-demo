#include "background_thread.h"

background_thread::result::result()
    : incomplete(false)
{}

background_thread::background_thread()
    : number_to_factor(0)
    , quit(false)
    , callback_queued(false)
    , cancel(false)
    , thread([this]
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lg(m);
            has_work_cv.wait(lg, [this]
            {
                return number_to_factor != 0 || quit;
            });
            
            if (quit)
                break;

            uint64_t n = number_to_factor;

            current_result.incomplete = true;
            current_result.factors.clear();
            queue_callback();

            lg.unlock();
            factor(n);
            lg.lock();

            current_result.incomplete = cancel.load();
            queue_callback();

            if (!cancel.load())
                number_to_factor = 0;
            cancel.store(false);
        }
    })
{}

background_thread::~background_thread()
{
    cancel.store(true);
    {
        std::unique_lock<std::mutex> lg(m);
        quit = true;
        has_work_cv.notify_all();
    }
    thread.join();
}

void background_thread::set_number_to_factor(uint64_t n)
{
    std::unique_lock<std::mutex> lg(m);
    if (number_to_factor != 0)
        cancel.store(true);
    number_to_factor = n;
    has_work_cv.notify_all();
}

background_thread::result background_thread::get_result() const
{
    return current_result;
}

void background_thread::factor(uint64_t a)
{
    uint64_t i = 2;

    if (a == 0)
        return;

    while (a != 1)
    {
        if (cancel.load())
            return;

        if ((a % i) == 0)
        {
            {
                std::unique_lock<std::mutex> lg(m);
                current_result.factors.push_back(i);
                queue_callback();
            }
            a /= i;
        }
        else
            ++i;
    }
}

void background_thread::queue_callback()
{
    if (callback_queued)
        return;

    QMetaObject::invokeMethod(this, "callback", Qt::QueuedConnection);
}

void background_thread::callback()
{
    {
        std::unique_lock<std::mutex> lg(m);
        callback_queued = false;
    }

    emit result_changed();
}
