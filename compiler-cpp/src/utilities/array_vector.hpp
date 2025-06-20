#include <tuple>
#include <array>
#pragma once


template <typename T>
class array_vector {
    public:
        array_vector(){ this->i_top = -1; this->array = nullptr;}
        ~array_vector() {if (i_top > 0) free(this->array); }

        void push_back(T new_val) {
            this->make_room();
            this->array[i_top] = new_val;
        }
        T& expand() {
            this->make_room();
            return this->array[this->i_top];
        }

        inline T* begin() { return this->array; }
        inline T* end() { return this->array + this->i_top; }

        size_t size() {return this->i_top + 1;};
        size_t get_count() {return this->i_top;}
        inline T& top() { return this->array[this->i_top]; }

        inline T& operator[](int index) { return this->array[index]; }
        inline T& operator()() {return this->array[this->i_top];}
        inline array_vector& operator=(T val) { this->push_back(val); return this;}

        
    private:
        size_t i_top;
        T* array;
        inline void make_room(){ 
            this->i_top++;
            if (this->i_top == 0) {
                this->array = (T*) malloc(sizeof(T));
            } else {
                this->array = (T*) realloc(this->array, sizeof(T)*(this->i_top + 1));
            }
        }
};