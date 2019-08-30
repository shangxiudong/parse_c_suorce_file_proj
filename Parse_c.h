#ifndef PARSE_C_H
#define PARSE_C_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cstddef>

#define STR_VOID ""//@todo
#define STR_WRONG "" //@todo
#define NULL_BEGIN '.'

class To_h; /* 未实现 */
class Sort_h; /* 未实现 */
class Parse_c /* 解析一个C源文件 */
{
    friend To_h;
    friend Sort_h;
public:
    Parse_c(const std::string& path_str, std::istream& sort_vector);
    virtual ~Parse_c();
    void show_mem()const;
protected:
    /*获取文件头部说明，直到出现程序正文(相对于注释)*/
    bool check_void(const std::string& path_str) /*字符串判空*/
    {
        if(path_str.begin() == path_str.end())
            return true;
        return false;
    }
    /*截取路径文件的最后文件名*/
    std::string path_str_to_name(const std::string& path_str);
    /*截取文件路径*/
    std::string path_str_to_path(const std::string& path_str);
    /*解析注释,并返回字符串*/
    std::string pass_illustrate(std::istream& in);
    /*解析字符串，并返回该字符串*/
    std::string pass_string(std::istream& in);
    /*跳过并返回代码段,代码端以c_begin标识开始，以c_end 标识结束*/
    std::string pass_codeblock(std::istream& in, const char c_begin, const char c_end);
    /*解析正文，将函数、全局变量、宏定义*/
    void Parse_main_body(std::istream& in);
    /*将所有头文件纳入*/
    void to_header(std::istream& in);
    /*将所有宏定义存储，因为宏定义的预处理块影响全局结构，单独处理*/
    void to_macro_definition(std::istream& in, std::string& discrip); //第二个参数用于传递之前的注释
    void switch_variable_function(std::istream& in, std::string& body);
    void to_function(std::istream& in, std::string& pre_body);
    void to_variable(std::istream& in, std::string& pre_body);
    void to_undentified(std::istream& in, std::string& pre_body);
private:
    std::string file_path;
    std::string file_name;
    std::multimap<std::string, std::string> keyword; /*不同 类型的C语言关键字及用户自定义类型*/
    std::string discription; /* 本文件的说明 */
    std::map<size_t, std::string> macro_definition; /* 宏定义 */
    std::vector<std::string> header; /* 头文件 */
    std::vector<std::pair<std::string, std::string> > pre_pro; /*预处理-变量/函数/预处理...*/
    std::map<std::string, std::string> func_map; /*函数声明-具体函数*/
    std::map<std::string, std::string> g_variable_map;/*全局变量声明-具体变量定义*/
    std::vector<std::string> key_sort; /* 分类依据 */
    std::vector<std::string> un_identified; /* 未识别的元素，为正常结束，就遇到了#endif,如extern C*/
};
#endif //PARSE_C_H

