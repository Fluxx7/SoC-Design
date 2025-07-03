#pragma once
#include <new>
#include <utility>

template <typename T>
class array_vector {
    public:
        array_vector() { 
            i_top = -1; 
            array = nullptr;
        }
        array_vector(array_vector& val) noexcept { 
            this->i_top = val.get_count();

            array = static_cast<T*>(::operator new[](sizeof(T)*(i_top+1)));
            for(int i = 0; i <= i_top; ++i ){
                new (&array[i]) T(val[i]);
            }
        }
        array_vector(array_vector&& val) noexcept { 
            this->i_top = val.i_top;
            this->array = val.array;
            
            val.i_top = -1;
            val.array = nullptr;
        }
        ~array_vector() { 
            clear();
            ::operator delete[](array);
        }

        template <typename U>
        void push_back(U&& new_val) requires std::is_constructible_v<T, U&&> {
            this->make_room();
            new (&array[i_top]) T(std::forward<U>(new_val));
        }

        T& expand() {
            this->make_room();
            new (&array[i_top]) T;
            return this->array[this->i_top];
        }

        T* begin() { return this->array; }
        T* end() { return this->array + size(); }

        size_t size() const {return static_cast<size_t>(this->i_top + 1);};
        int get_count() const {return this->i_top;}
        T& top() { return this->array[this->i_top]; }

        T& operator[](int index) { return this->array[index]; }
        const T& operator[](int index) const { return this->array[index]; }
        T& operator()() {return this->array[this->i_top];}

        template <typename U>
        array_vector& operator+=(U&& val) requires std::is_constructible_v<T, U&&> { 
            this->push_back(std::forward<U>(val)); return *this;
        }

        array_vector& operator=(array_vector& val) noexcept { 
            this->i_top = val.get_count();

            T* new_array = static_cast<T*>(::operator new[](sizeof(T)*(i_top+1)));
            for(int i = 0; i <= i_top; ++i ){
                new (&new_array[i]) T(val[i]);
                array[i].~T();
            }
            ::operator delete[](array);
            array = new_array;
            return *this;
        }

        array_vector& operator=(array_vector&& val) noexcept { 
            if (this != &val) {
                ::operator delete[](this->array);
                this->i_top = val.i_top;
                this->array = val.array;
                
                val.i_top = -1;
                val.array = nullptr;
            }
            return *this;
        }

        void clear() {
            for (int i = 0; i <= i_top; ++i) {
                array[i].~T();
            }
            i_top = -1;
        }
        
    private:
        int i_top;
        T* array;
        void make_room(){ 
            this->i_top++;

            T* new_array = static_cast<T*>(::operator new[](sizeof(T)*(i_top+1)));
            for(int i = 0; i < i_top; ++i ){
                new (&new_array[i]) T(std::move(array[i]));
                array[i].~T();
            }
            ::operator delete[](array);
            array = new_array;
        }
};

