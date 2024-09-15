#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

class Observer {
public:
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
    virtual ~Observer() = default;
};

class Observable {
public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void warning(const std::string& message) const {
        notifyWarning(message);
    }

    void error(const std::string& message) const {
        notifyError(message);
    }

    void fatalError(const std::string& message) const {
        notifyFatalError(message);
    }

private:
    std::vector<Observer*> observers;

    void notifyWarning(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onWarning(message);
            }
        }
    }

    void notifyError(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onError(message);
            }
        }
    }

    void notifyFatalError(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onFatalError(message);
            }
        }
    }
};

class WarningObserver : public Observer {
public:
    void onWarning(const std::string& message) override {
        std::cout << "Предупреждение: " << message << std::endl;
    }
};

class ErrorObserver : public Observer {
public:
    explicit ErrorObserver(const std::string& filePath) : filePath(filePath) {}

    void onError(const std::string& message) override {
        std::ofstream file(filePath, std::ios::app);
        if (file.is_open()) {
            file << "Ошибка: " << message << std::endl;
            file.close();
        }
    }

private:
    std::string filePath;
};

class FatalErrorObserver : public Observer {
public:
    explicit FatalErrorObserver(const std::string& filePath) : filePath(filePath) {}

    void onFatalError(const std::string& message) override {
        // Печать в консоль
        std::cout << "Критическая ошибка: " << message << std::endl;

        // Запись в файл
        std::ofstream file(filePath, std::ios::app);
        if (file.is_open()) {
            file << "Критическая ошибка: " << message << std::endl;
            file.close();
        }
    }

private:
    std::string filePath;
};

int main() {
    setlocale(LC_ALL, "RU");
    Observable observable;

    WarningObserver warningObserver;
    ErrorObserver errorObserver("errors.log");
    FatalErrorObserver fatalErrorObserver("fatal_errors.log");

    observable.addObserver(&warningObserver);
    observable.addObserver(&errorObserver);
    observable.addObserver(&fatalErrorObserver);

    observable.warning("Это предупреждение");
    observable.error("Это ошибка");
    observable.fatalError("Это критическая ошибка");

    observable.removeObserver(&errorObserver);

    observable.error("Эта ошибка не зафиксирована");

    return 0;
}
