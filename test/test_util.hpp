#pragma once

#define RESET_TIME setTime(0, 0, 0, 1, 1, 2000)
#define SET_MORNING_TRIGGER_TIME setTime(8, 0, 0, 1, 1, 2000)
#define SET_EVENING_TRIGGER_TIME setTime(18, 0, 0, 1, 1, 2000)

unsigned long mock_cpu_ticks = 60L * 60L * 1000L;

void mock_millis_elapse(unsigned long ms)
{
    mock_cpu_ticks += ms;
}

unsigned long mock_millis()
{
    return mock_cpu_ticks;
}

// Callback helper
template<typename T>
struct ActualType {
    typedef T type;
};
template<typename T>
struct ActualType<T*> {
    typedef typename ActualType<T>::type type;
};

template<typename T, unsigned int n,typename CallerType>
struct Callback;

template<typename Ret, typename ... Params, unsigned int n,typename CallerType>
struct Callback<Ret(Params...), n,CallerType> {
    typedef Ret (*ret_cb)(Params...);
    template<typename ... Args>
    static Ret callback(Args ... args) {
        func(args...);
    }

    static ret_cb getCallback(std::function<Ret(Params...)> fn) {
        func = fn;
        return static_cast<ret_cb>(Callback<Ret(Params...), n,CallerType>::callback);
    }

    static std::function<Ret(Params...)> func;

};

template<typename Ret, typename ... Params, unsigned int n,typename CallerType>
std::function<Ret(Params...)> Callback<Ret(Params...), n,CallerType>::func;

#define GETCB(ptrtype,callertype) Callback<ActualType<ptrtype>::type,__COUNTER__,callertype>::getCallback