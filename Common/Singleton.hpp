#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <QMutex>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator==(const Singleton&) = delete;

public:
    static T* instance()
    {
        if (s_instance == nullptr) {
            QMutexLocker locker(&s_mutex);
            if (s_instance == nullptr) {
                s_instance = new T();
            }
        }

        return s_instance;
    }
    void DestoryInstance()
    {
        QMutexLocker locker(&s_mutex);
        if (s_instance != nullptr) {
            delete s_instance;
            s_instance = nullptr;
        }
    }

private:
    static QMutex s_mutex;
    static T* s_instance;
};

#endif // SINGLETON_HPP
