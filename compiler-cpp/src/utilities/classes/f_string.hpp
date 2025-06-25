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
        f_string(const f_string& input) {
            this->size = input.length();
            this->content = static_cast<char*>(::operator new[](this->size+1));
            std::memcpy(this->content, input.get_content(), this->size+1);
        }
        f_string(f_string&& input) {
            this->size = input.size;
            this->content = input.content;

            input.size = 0;
            input.content = nullptr;
        }
        f_string(const char* input) {
            this->size = strlen(input);
            this->content = static_cast<char*>(::operator new[](this->size+1));
            std::memcpy(this->content, input, this->size);
            this->content[size] = '\0';
        }
        f_string(size_t length) {
            this->size = length;
            this->content = static_cast<char*>(::operator new[](this->size+1));
        }
        ~f_string(){
            if (this->content != nullptr) ::operator delete[](this->content);
            this->content = nullptr;
            this->size = 0;
        }

        inline char* begin() {return this->content;}
        inline char* end() {return this->content + this->size;}

        f_string& operator=(const char* new_str){
            if (new_str == this->content) {
                return *this;
            }
            resize(strlen(new_str));
            std::memcpy(this->content, new_str, this->size);
            return *this;
        }
        f_string& operator=(const f_string& new_str){
            if (this == &new_str) {
                return *this;
            }
            this->size = new_str.length();
            if (this->content != nullptr) {
                ::operator delete[](this->content);
            }
            this->content = static_cast<char*>(::operator new[](this->size+1));
            std::memcpy(this->content, new_str.get_content(), this->size+1);
            return *this;
        }
        f_string& operator=(f_string&& input) {
            if (this != &input) {
                if (this->content != nullptr) {
                    ::operator delete[](this->content);
                }
                this->size = input.size;
                this->content = input.content;

                input.size = 0;
                input.content = nullptr;
            }
            return *this;
        }
        f_string& operator+=(const char* new_str){
            resize(strlen(new_str)+this->size);
            std::memcpy(this->content + this->size - 1, new_str, strlen(new_str));
            return *this;
        }
        f_string& operator+=(const f_string& new_str){
            int orig_size = this->size;
            resize(new_str.length()+this->size);
            std::memcpy(this->content + orig_size, new_str.get_content(), new_str.length());
            this->content[size] = '\0';
            return *this;
        }
        f_string& operator+=(char new_char){
            if (new_char == '\0') {
                this->content[size] = new_char;         
                return *this;
            }
            resize(this->size+1);
            this->content[size-1] = new_char;
            
            return *this;
        }
        f_string operator+(const char* new_str){
            f_string new_fstr(*this);
            new_fstr += new_str;
            return new_fstr;
        }
        f_string operator+(const f_string& new_str){
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
        char operator[](int index) const {
            return this->content[index];
        }
        bool operator==(char* comp) const {
            return strcmp(this->content, comp) == 0;
        }
        bool operator!=(char* comp) const {
            return !(*this == comp);
        }
        bool operator==(const f_string& comp) const {
            return strcmp(this->content, comp.get_content()) == 0;
        }
        bool operator==(const char* comp) const {
            return strcmp(this->content, comp) == 0;
        }
        bool operator!=(const f_string& comp) const {
            return !(*this == comp);
        }
        operator char*() const {
            return this->content;
        }
        bool operator!=(const char* comp) const {
            return strcmp(this->content, comp) != 0;
        }
        inline char* get_content() {
            return this->content;
        }
        inline const char* get_content() const{
            return this->content;
        }
        inline size_t length() const {
            return this->size;
        }

        inline f_string slice(int index) const {
            return f_string(this->content + index);
        } 
        inline f_string slice(const char* index) const {
            return f_string(this->content + strlen(index));
        } 

        int ssplit(f_string& output, int* iindex, const char* delim) const{
            int delim_match = 0;
            int local_index = 0;
            int& index = iindex == nullptr ? local_index : *iindex;
            int orig_index = iindex == nullptr ? 0 : *iindex;
            char* out_str = new char[this->size+1];
            if (index >= this->size){
                return 0;
            }
            while(1){
                if (this->content[index] == '\0') {
                    out_str[index-orig_index] = '\0';
                    output = out_str;
                    index = orig_index;
                    return 0;
                } else {
                    if (this->content[index] != delim[delim_match]){
                        if (delim_match > 0) {
                            delim_match = 0;
                        }
                        out_str[index-orig_index] = this->content[index];
                    } else {
                        delim_match++;
                        out_str[index-orig_index] = this->content[index];
                        if (delim[delim_match] == '\0') {
                            out_str[(index-orig_index)-delim_match+1] = '\0';
                            (index)++;
                            output = out_str;
                            return 1;
                        } 
                    }
                    (index)++;
                }
            }
        }  

        int ssplit(char* output, int* iindex, const char* delim) const{
            int delim_match = 0;
            int local_index = 0;
            int& index = iindex == nullptr ? local_index : *iindex;
            int orig_index = iindex == nullptr ? 0 : *iindex;
            bool out = output == nullptr ? false : true;
            if (index >= this->size){
                return 0;
            }
            while(1){
                if (this->content[index] == '\0') {
                    index = orig_index;
                    return 0;
                } else {
                    if (this->content[index] != delim[delim_match]){
                        if (delim_match > 0) {
                            delim_match = 0;
                        }
                        if(out) output[index-orig_index] = this->content[index];
                    } else {
                        delim_match++;
                        if (out) output[index-orig_index] = this->content[index];
                        if (delim[delim_match] == '\0') {
                            if (out) output[(index-orig_index)-delim_match+1] = '\0';
                            (index)++;
                            return 1;
                        } 
                    }
                    (index)++;
                }
            }
        }  

        f_string sclean_i(char character, int count) const{
            f_string out;
            char* out_str = new char[this->size+1];
            int index = 0,tok_index = 0;
            int add_char = 0;
            while(1){
                if (this->content[index] == '\0' || this->content[index] == '\n') {
                    out_str[tok_index] = '\0';
                    out = out_str;


                    return out;
                } else {
                    if (this->content[index] != character){
                        if (add_char == 0){
                            add_char = 1;
                        }

                        out_str[tok_index] = this->content[index];
                        tok_index++;
                    } else {
                        if (add_char == 1){
                            for (int i = 0; i < count; i++){
                                out_str[tok_index] = character;
                                tok_index++;
                            }
                            add_char = 0;
                        }
                    }
                    index++;
                }
            }
        }
        f_string sclean() const {
            return sclean_i(' ', 0);   
        }
        int smatch(const char* match) const {
            for (int i = 0; i < strlen(match); i++){
                if (this->content[i] != match[i]) {
                    return 0;
                }
            }
            return 1;
        }
        char* fgets(size_t linesize, FILE* file) {
            char* out = static_cast<char*>(::operator new[](linesize));
            char* result = std::fgets(out, linesize, file);
            if (this->content != nullptr){
                ::operator delete[](this->content);
                this->content = nullptr;
                this->size = 0;
            }
            if (result == NULL) {
                this->content = nullptr;
                this->size = 0;
                delete(out);
                return NULL;
            }
            resize(strlen(out));
            std::memcpy(this->content, out, strlen(out));
            return result;
        }
        
    private:
        size_t size;
        char* content;
        void resize(size_t new_size){  
            char* new_content = new char[new_size+1];
            if (this->content != nullptr) {
                std::memcpy(new_content, this->content, (this->size > new_size ? new_size : this->size) + 1);
                ::operator delete[](this->content);
                
            }
            this->content = new_content;
            this->size = new_size;
            this->content[size] = '\0';
        }
};

inline f_string operator+(const char * lhs, f_string rhs) {
    f_string new_fstr(lhs);
    new_fstr += rhs;
    return new_fstr;
}

namespace std {
    template <>
    struct formatter<f_string, char> : std::formatter<char*> {

        auto format(const f_string& val, std::format_context& ctx) const {
            return std::formatter<const char*>::format(val.get_content(), ctx);
        }
    };
}