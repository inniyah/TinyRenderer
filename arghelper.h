/*
 * Argument Helper
 *
 * Daniel Russel drussel@alumni.princeton.edu
 * Stanford University
 *
 * Support for long long and unsigned long long by
 * Raoul Steffen <R-Steffen@gmx.de>
 *
 *
 * This software is not subject to copyright protection and is in the
 * public domain. Neither Stanford nor the author assume any
 * responsibility whatsoever for its use by other parties, and makes no
 * guarantees, expressed or implied, about its quality, reliability, or
 * any other characteristic.
 */

#pragma once

#ifndef ARGHELPER_H_EF03189C_8A5F_11EA_ACC8_10FEED04CD1C
#define ARGHELPER_H_EF03189C_8A5F_11EA_ACC8_10FEED04CD1C

#ifdef _MSC_VER
#define my_sscanf sscanf_s
#else
#define my_sscanf sscanf
#endif
#include <sstream>

#include <vector>
#include <map>
#include <list>
#include <string>
#include <string.h>

namespace dsr{
    extern bool verbose, VERBOSE;


    class Argument_helper{
    private:
        class Argument_target;


        class FlagTarget;
        class DoubleTarget;
        class IntTarget;
        class UIntTarget;
        class LongLongTarget;
        class ULongLongTarget;
        class StringTarget;
        class CharTarget;
        class StringVectorTarget;

    public:
        Argument_helper();
        void new_flag(char key, const char *long_name, const char *description,bool &dest);

        void new_string( const char *arg_description, const char *description, std::string &dest);
        void new_named_string(char key, const char *long_name,
            const char *arg_description,
            const char *description,  std::string &dest);
        void new_optional_string( const char *arg_description, const char *description, std::string &dest);

        void new_int( const char *arg_description, const char *description, int &dest);
        void new_named_int(char key, const char *long_name,const char *value_name,
            const char *description,
            int &dest);
        void new_optional_int(const char *value_name,
            const char *description,
            int &dest);

        void new_unsigned_int(const char *value_name, const char *description,
            unsigned int &dest);
        void new_optional_unsigned_int(const char *value_name, const char *description,
            unsigned int &dest);
        void new_named_unsigned_int(char key, const char *long_name,
            const char *value_name, const char *description,
            unsigned int &dest);

        void new_long_long( const char *arg_description, const char *description, long long &dest);
        void new_named_long_long(char key, const char *long_name,const char *value_name,
            const char *description,
            long long &dest);
        void new_optional_long_long(const char *value_name,
            const char *description,
            long long &dest);

        void new_unsigned_long_long(const char *value_name, const char *description,
            unsigned long long &dest);
        void new_optional_unsigned_long_long(const char *value_name, const char *description,
            unsigned long long &dest);
        void new_named_unsigned_long_long(char key, const char *long_name,
            const char *value_name, const char *description,
            unsigned long long &dest);

        void new_double(const char *value_name,
            const char *description,
            double &dest);

        void new_named_double(char key, const char *long_name,const char *value_name,
            const char *description,
            double &dest);
        void new_optional_double(const char *value_name,
            const char *description,
            double &dest);

        void new_char(const char *value_name,
            const char *description,
            char &dest);
        void new_named_char(char key, const char *long_name,const char *value_name,
            const char *description,
            char &dest);
        void new_optional_char(const char *value_name,
            const char *description,
            char &dest);


        void new_named_string_vector(char key, const char *long_name,
            const char *value_name, const char *description,
            std::vector<std::string> &dest);


        void set_string_vector(const char *arg_description, const char *description, std::vector<std::string> &dest);

        void set_author(const char *author);

        void set_description(const char *descr);

        void set_version(float v);
        void set_version(const char *str);

        void set_name(const char *name);

        void set_build_date(const char *date);


        void process(int argc, const char **argv);
        void process(int argc, char **argv){
            process(argc, const_cast<const char **>(argv));
        }
        void write_usage(std::ostream &out) const;
        void write_values(std::ostream &out) const;

        ~Argument_helper();
    protected:
        typedef std::map<char, Argument_target*> SMap;
        typedef std::map<std::string, Argument_target*> LMap;
        typedef std::vector<Argument_target*> UVect;
        // A map from short names to arguments.
        SMap short_names_;
        // A map from long names to arguments.
        LMap long_names_;
        std::string author_;
        std::string name_;
        std::string description_;
        std::string date_;
        float version_;
        bool seen_end_named_;
        // List of unnamed arguments
        std::vector<Argument_target*> unnamed_arguments_;
        std::vector<Argument_target*> optional_unnamed_arguments_;
        std::vector<Argument_target*> all_arguments_;
        std::string extra_arguments_descr_;
        std::string extra_arguments_arg_descr_;
        std::vector<std::string> *extra_arguments_;
        std::vector<Argument_target*>::iterator current_unnamed_;
        std::vector<Argument_target*>::iterator current_optional_unnamed_;
        void new_argument_target(Argument_target*);
        void handle_error() const;
    private:
        Argument_helper(const Argument_helper &){};
        const Argument_helper& operator=(const Argument_helper &){return *this;}
    };

}

#endif // ARGHELPER_H_EF03189C_8A5F_11EA_ACC8_10FEED04CD1C
