#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() : is_initialized_(false), value_(nullptr)
    {}

    Optional(const T& value) {
        value_ = new (data_) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) {
        value_ = new (data_) T(std::move(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.is_initialized_) {
            value_ = new (data_) T(*other.value_);
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) {
        if (other.is_initialized_) {
            value_ = new (data_) T(std::move(*other.value_));
            is_initialized_ = true;
        }
    }

    Optional& operator=(const T& value) {
        if (is_initialized_) {
            *value_ = std::move(value);
        }
        else {
            value_ = new (data_) T(value);
            is_initialized_ = true;
        }

        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (is_initialized_) {
            *value_ = std::move(rhs);
        }
        else {
            value_ = new (data_) T(std::move(rhs));
            is_initialized_ = true;
        }
       
        return *this;
    }

    Optional& operator=(const Optional& rhs) {
        if (&rhs != this) {
            if (!rhs.is_initialized_) {
                Reset();
            }
            else if (is_initialized_) {
                *value_ = *rhs.value_;
            }
            else {
                value_ = new (data_) T(*rhs.value_);
                is_initialized_ = true;
            }
        }

        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (&rhs != this) {
            if (!rhs.is_initialized_) {
                Reset();
            }
            else if (is_initialized_) {
                *value_ = std::move(*rhs.value_);
            }
            else {
                value_ = new (data_) T(std::move(*rhs.value_));
                is_initialized_ = true;
            }
        }

        return *this;
    }

    ~Optional() {
        Reset(); 
    }

    bool HasValue() const {
        return is_initialized_;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*() {
        return *value_;
    }

    const T& operator*() const {
        return *value_;
    }

    T* operator->() {
        return value_;
    }

    const T* operator->() const {
        return value_;
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return *value_;
    }

    const T& Value() const {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return *value_;
    }

    void Reset() {
        
        if (is_initialized_) {
            value_->~T();
            value_ = nullptr;
            is_initialized_ = false;
        }
    }

    template <typename... Ts>
    void Emplace(Ts&& ...vs) {
        if (is_initialized_) {
            value_->~T();
            
        }
        value_ = new (data_) T(std::forward<Ts>(vs)...);
        is_initialized_ = true;
    }

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
    T* value_;
};

