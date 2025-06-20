#include <string.h>
#include <cstdlib>
#include <memory>
#include <format>
#include <iostream>
#include <utility>
#include <vector>
#include <type_traits>

#pragma once


class f_string {
    friend f_string operator+(const char * lhs, f_string rhs);
    public:
        f_string() {
            this->size = 0;
            this->content = nullptr;
        }
        f_string(const char* input) {
            this->size = strlen(input);
            this->content = (char*) malloc(sizeof(char)*(this->size+1));
            strcpy(this->content, input);
        }
        f_string(size_t length) {
            this->size = length;
            this->content = (char*) malloc(sizeof(char)*(this->size+1));
        }
        ~f_string(){
            if (this->content != nullptr) free(this->content);
        }

        inline char* begin() {return this->content;}
        inline char* end() {return this->content + this->size;}

        f_string& operator=(const char* new_str){
            if (new_str == this->content) {
                return *this;
            }
            resize(strlen(new_str));
            strcpy(this->content, new_str);
            return *this;
        }
        f_string& operator+=(const char* new_str){
            resize(strlen(new_str)+this->size);
            strcat(this->content, new_str);
            return *this;
        }
        f_string& operator+=(char new_char){
            resize(this->size+1);
            this->content[size] = new_char;
            return *this;
        }
        f_string operator+(const char* new_str){
            f_string new_fstr(*this);
            new_fstr += new_str;
            return new_fstr;
        }
        f_string operator+(char new_char){
            f_string new_fstr(*this);
            new_fstr += new_char;
            return new_fstr;
        }
        char& operator[](int index){
            return this->content[index];
        }
        bool operator==(char* comp) const {
            return strcmp(this->content, comp) == 0;
        }
        bool operator!=(char* comp) const {
            return !(*this == comp);
        }
        bool operator==(f_string comp) const {
            return strcmp(this->content, comp) == 0;
        }
        bool operator!=(f_string comp) const {
            return !(*this == comp);
        }
        operator char*() {
            return this->content;
        }
        inline char* get_content() {
            return *this;
        }
        inline size_t length() const {
            return this->size;
        }

        constexpr inline f_string slice(int index) {
            return f_string(this->content + index);
        } 
        constexpr inline f_string slice(const char* index) {
            return f_string(this->content + strlen(index));
        } 

        int ssplit(char* output, int& iindex, const char* delim){
            auto& src = this->content;
            int delim_match = 0;
            std::shared_ptr<int> index(&iindex);
            int orig_index;
            bool out = output == nullptr ? true : false;
            if (index == nullptr) {
                index = std::make_shared<int>(0);
                orig_index = 0;
            } else {
                if (*index >= strlen(src)){
                    return 0;
                }
                orig_index = iindex;
                //printf("Index: %d\n", *index);
            }
            while(1){
                if (src[*index] == '\0') {
                    *index = orig_index;
                    return 0;
                } else {
                    if (src[*index] != delim[delim_match]){
                        if (delim_match > 0) {
                            delim_match = 0;
                        }
                        if(out) output[*index-orig_index] = src[*index];
                    } else {
                        delim_match++;
                        if (out) output[*index-orig_index] = src[*index];
                        if (delim[delim_match] == '\0') {
                            if (out) output[(*index-orig_index)-delim_match+1] = '\0';
                            (*index)++;
                            return 1;
                        } 
                    }
                    (*index)++;
                }
            }
        }  

        f_string sclean_i(char character, int count){
            f_string out;
            int index = 0, tok_index = 0;
            int add_char = 0;
            while(1){
                if (this->content[index] == '\0' || this->content[index] == '\n') {
                    out[tok_index] = '\0';


                    return out;
                } else {
                    if (this->content[index] != character){
                        if (add_char == 0){
                            add_char = 1;
                        }

                        out[tok_index] = this->content[index];
                        tok_index++;
                    } else {

                        if (add_char == 1){
                            for (int i = 0; i < count; i++){
                                out[tok_index] = character;
                                tok_index++;
                            }
                            add_char = 0;
                        }
                    }
                    index++;
                }
            }
            
        }
        f_string sclean(){
            return sclean_i(' ', 0);   
        }
        int smatch(const char* match){
            for (int i = 0; i < strlen(match); i++){
                if (this->content[i] != match[i]) {
                    return 0;
                }
            }
            return 1;
        }
        
    private:
        size_t size;
        char* content;
        void resize(size_t new_size){
            this->size = new_size;
            if (this->content != nullptr) {
                this->content = (char*) realloc(this->content, sizeof(char)*(new_size+1));
            } else {
                this->content = (char*) malloc(sizeof(char)*(new_size+1));
            }
        }
};

f_string operator+(const char * lhs, f_string rhs) {
    f_string new_fstr(lhs);
    new_fstr += rhs;
    return new_fstr;
}

namespace std {
    template <>
    struct formatter<f_string, char> : std::formatter<char*> {

        auto format(f_string& val, std::format_context& ctx) const {
            return inner_formatter.format(val.get_content(), ctx);
        }

        private:
            std::formatter<char*> inner_formatter;
    };
}