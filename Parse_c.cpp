#include "Parse_c.h"
#include <cstdio>

using namespace std;

Parse_c::Parse_c(const std::string& path_str, std::istream& sort_vector)
{
    file_name = path_str_to_name(path_str);
    file_path = path_str_to_path(path_str);
    ifstream in(path_str);
    discription = pass_illustrate(in);
    Parse_main_body(in);
}
Parse_c::~Parse_c()
{
//dtor
}

void Parse_c::show_mem() const
{
//    cout<< discription << endl;
//    cout<< file_name << endl;
//    cout<< file_path << endl;
//    for(auto i:header)
//        cout<< "header:"<< i << endl;
    string store_path = file_path;
    store_path.append(file_name);
    store_path.append("_func.c");
    ofstream out(store_path);
    /***************打印函数***************/
    for(const auto& i: func_map) {
        for (const auto& il: i.second) {
            out<< il;
        }
        out<< endl;
        out<<  "/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/"  << endl;
        //cout<< "function name: "<< i.first<< endl;
    }
    //cout<< func_map.size() << endl;
    out.close();
    /************打印全局对象**************/
    store_path.assign(file_path);
    store_path.append(file_name);
    store_path.append("_variable.c");
    out.open(store_path);
    for(const auto& i: g_variable_map) {
        for (const auto& il: i.second) {
            out << il;
        }
        out << endl;
        out << "/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/" << endl;
        //cout << "variable name:" << i.first << endl;
    }
    out.close();
    /*************打印宏定义**************/
    store_path.assign(file_path);
    store_path.append(file_name);
    store_path.append("_macro_definition.c");
    out.open(store_path);
    for(const auto& i: macro_definition) {
        for (const auto& il: i.second) {
            out << il;
        }
    }
    out << endl;
    out << "/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+***/" << endl;
    out.close();

    /********** 打印未定义 ******************/
    store_path.assign(file_path);
    store_path.append(file_name);
    store_path.append("_unidentified.c");
    out.open(store_path);
    for(const auto& i: un_identified) {
        out << i << endl;
        out << "/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/" << endl;
    }
    out.close();
    /*****************************************************/
    //cout << g_variable_map.size() << endl;
    for(const auto& i: macro_definition) {
        for (const auto& il: i.second) {
            //cout << il;
        }
        //cout << endl;
    }
}

/********************************
*截取路径文件的最后文件名
********************************/
string Parse_c::path_str_to_name(const string& path_str)
{
    if(check_void(path_str))
        return STR_VOID; /* @todo */
    string::const_iterator it_first;
    string::const_iterator it_second;
    for(it_first = path_str.end(); *--it_first != '\\' && it_first != path_str.begin(););
    for(it_second = path_str.end(); *--it_second !='.' && it_second != path_str.begin(););
    string s(++it_first, it_second);
    return s;
}

/********************************
*截取文件路径
********************************/
string Parse_c::path_str_to_path(const string& path_str)
{
    if(check_void(path_str)) return STR_VOID;
    auto it_end = path_str.end();
    while(*--it_end != '\\' && it_end != path_str.begin());
    string s(path_str.begin(), ++it_end);
    return s;
}

/********************************
*返回[连在一起的注释(包括连续注释和单行注释)]的字符
*并将流(in)指向下一个代码开始位置
********************************/
string Parse_c::pass_illustrate(std::istream& in)
{
    string buf;
    char c;
    in >> c;//识别第一个\n非空；
    if(c == '/') {
        c = (char)in.peek();
        if (c == '*') {
            in.get(c);
            buf.append("/*");
            while(in.get(c)) {
                buf.push_back(c);
                if(c =='*') {
                    c = (char)in.peek();
                    if (c== '/') {
                        in.get(c);
                        buf.push_back(c);
                        break;
                    }
                } else if (in.eof()) {
                    cerr << "no '*/' until read file EOF" << endl;
                    return STR_WRONG; //@todo
                }
            }
            buf.push_back('\n');
            // buf += pass_illustrate(in);
        } else if (c == '/') {
            in.get(c);
            buf.append("//");
            string temp;
            getline(in, temp);
            in.unget();
            buf.append(temp);
            buf.push_back('\n');
            //buf += pass_illustrate(in)
        } else {
            buf.push_back('/');
        }
    } else {
        in.unget();
    }
    return buf;
}

/********************************
*返回一串字符(包括""对本身)的字符串
*并将流(in)指向下一个代码开始位置
********************************/
std::string Parse_c::pass_string(std::istream& in)
{
    string s; /*要返回的字符串*/
    char c; /*识别触发的字符*/
    in >> c;
    if (c == '"') {
        s.push_back(c);
        while(in.get(c)) {
            if (c == '\\') {
                s.push_back(c);
                in.get(c);
                s.push_back(c);
            } else if(c == '"') {
                s.push_back(c);
                return s;
            } else if(in.eof()) {
                cerr << "read file's end, but there is no end of a string!" << endl;
                return STR_WRONG; //未对此进行操作 @todo
            } else {
                s.push_back(c);
            }
        }
    } else if(c == '\'') {
        s.push_back(c);
        while(in.get(c)) {
            if(c =='\\') {
                s.push_back(c);
                in.get(c); /* 如果读到'\'，则无条件录入一次 */
                s.push_back(c);
            } else if (c == '\'') {
                s.push_back(c);
                return s;
            } else if(in.eof()) {
                cerr << "read file's end, but there is no end of a <'>!" << endl;
                return STR_WRONG; //未对此进行操作 @todo
            } else {
                s.push_back(c);
            }
        }
    } else {
        in.unget();
    }
    return s;
}

/********************************
*跳过并返回一次代码段
*代码段以c_begin标识开始
*以c_end标识结束
********************************/
string Parse_c::pass_codeblock(std::istream& in, const char c_begin, const char c_end)
{
    string s;
    char c;
    in >> c;
    //cout << "function pass_codeblock activated, c_begin: " << c << "c_end: " << c_end << endl;
    if(c == c_begin || c_begin == NULL_BEGIN) {
        s.push_back(c);
        while(c = (char)in.peek()) {
            if(c == '/') {
                s += pass_illustrate(in);
            } else if(c == '"' || c == '\'') {
                s += pass_string(in);
            } else if (c == '\\') {
                in.get(c);
                s.push_back(c);
                in.get(c); /*读到'\'无条件读取一次后面的换行*/
                if(c != '\n') {
                    cerr << "expected '\n' after '\\'" << endl;
                    return STR_WRONG; /* @todo */
                }
                s.push_back(c);
            } else if (c == c_begin && c_begin != NULL_BEGIN) {
                s += pass_codeblock(in, c_begin, c_end); //
            } else if (c == c_end) {
                in.get(c);
                s.push_back(c);
                //cout << "out of loop pass_codeblock c: " << c << endl;
                break; /* 递归出口 */
            } else if (in.eof()) {
                cerr << "read file's end, but there is no end of a code bolck" << endl;
                //cout << "some error happens" << endl;
                return STR_WRONG;
            } else {
                in.get(c);
                s.push_back(c);
            }
        }
    } else {
        in.unget();
        return s;
    } //end if c == '#'
    return s;
}

/********************************
*从文件描述之后解析整个文件，解析入口
********************************/
void Parse_c::Parse_main_body(std::istream& in)
{
    //cout << "running Parse_main_body()"<< endl;
    char c;
    string buf;
    while(in >> c) {
        //cout << "index char:" << c << endl;
        switch (c) {
        case '/':
            in.unget();
            buf += pass_illustrate(in);
            break;
        case '#':
            //cout<< "main_body to macro_definition" << endl; //--
            in.unget();
            to_macro_definition(in, buf);
            buf.erase();
            break;
        default:
            //cout << "main_body to function or variable" << endl; //--
            in.unget();
            switch_variable_function(in, buf);
            buf.erase();
            break;
        }
    }
}

/********************************
*#include头文件
********************************/
void Parse_c::to_header(istream& in)
{
    string s;
    while(in >> s) {
        if (s == "#include") {
        string temp;
        getline(in, temp);
        s += temp;
        header.push_back(s);
        }
    }
}

/********************************
*识别一次宏定义，遇到宏定义结尾结束
*需要继续细化，目前只是将它识别出来
* @todo
********************************/
void Parse_c::to_macro_definition(std::istream& in, std::string& pre_body)
{
    //cout << "function to_macro_definition() running "<< endl;
    if((char)in.peek() == '#') {
        size_t mark = in.tellg(); //@todo
        pre_body += pass_codeblock(in, NULL_BEGIN,'\n');
        macro_definition.insert(make_pair(mark, pre_body));
    }
}

/********************************
*开关：指向录入变量的函数
*还是：指向录入函数的函数
* @todo case '['、'='、'{'\';'、...
********************************/
void Parse_c::switch_variable_function(std::istream& in, std::string& body)
{
    //cout << "switch_variable_function" << endl;
    int i;
    char c;
    while((i = in.get()) != EOF) {
        c = (char)i;
        if (ispunct(c)) {
            switch (c) {
            case ' ':
                body.push_back(c);
                break;
            case '_':
                body.push_back(c);
                break;
            case '(':
                body.push_back(c);
                to_function(in, body);
                return;
            case '*':
                body.push_back(c);
                break;
            case '=':
                body.push_back(c);
                //cout << "find '=' go to_variable()" << endl;
                //cout << body << endl;
                to_variable(in, body);
                return;
            case '[':
                body.push_back(c);
                //cout << "find '[' go to_variable()" << endl;
                //cout<< body << endl;
                to_variable(in, body);
                return;
            case '{':
                //cout << "find '{' go to_variable()" << endl;
                //cout << body << endl;
                body.push_back(c);
                to_variable(in, body);
                return;
            case ';':
                in.unget();
                to_variable(in, body);
                return;
            case '\n':
                body.push_back(c);
                break;
            case '/':
                in.unget();
                body += pass_illustrate(in);
                break;
            case '"':
                in.unget();
                body += pass_string(in);
                to_undentified(in, body);
                return;
            case '\'':
                in.unget();
                body += pass_string(in);
                break;
            case '\\':
                if(in.peek() != EOF && (char)in.peek() == '\n') {
                    body.push_back(c);
                } else {
                    body.push_back(c);
                    to_undentified(in, body);
                }
                break;
            default:
                in.unget();
                to_undentified(in, body);
                return;
            }
        } else {
            body.push_back(c);
        }
    }
}

/********************************
*存储函数
* @todo
*目前没有考虑函数接口部分存在注释情况
* bug: 函数定义和声明放在个文件的话，那么函数名相同，
* 代码是按照函数名做的map 索引，所以只会存储第一个函数索引
********************************/
void Parse_c::to_function(std::istream &in, std::string& pre_body)
{
    //cout << "to_function" << endl;
    int i;
    char c;
    auto it_end = pre_body.end();
    //将it_end 指向函数名的最后一个字符（过滤掉空格）
    while (it_end != pre_body.begin()) {
        if(isalnum(*it_end) || *it_end == '_') {
            ++it_end;
            break;
        }
        else
            --it_end;
    }
    //将it_begin 指向函数名的第一个字符
    auto it_begin = --it_end;
    while (it_begin != pre_body.begin()) {
        if(isalnum(*it_begin) || *it_begin == '_') {
            --it_begin;
        } else {
            ++it_begin;
            break;
        }
    }
    string name(it_begin, ++it_end);
    while((i = in.get()) != EOF) {
        c = (char)i;
        if (c =='{') {
            in.unget();
            pre_body += pass_codeblock(in,'{','}');
            string temp;
            getline(in, temp);
            in.unget();
            pre_body += temp; //如果}还有一段关于一段该函数的注释或其他
            break;
        } else if (c == '/') {
            pre_body += pass_illustrate(in);
        } else if (c=='"' || c=='\'') {
            pre_body += pass_string(in);
        } else if (c ==	';') {
            pre_body.push_back(c);
            string temp;
            getline(in, temp);
            in.unget();
            pre_body += temp;
            break;
        } else {
            pre_body.push_back(c);
        }
    }
    func_map.insert(make_pair(name, pre_body));
    return;
}

/********************************
*c里面变量
*只有用等于初始化或者未初始化
*@todo 目前没有考虑变量定义部分存在注释情况
********************************/
void Parse_c::to_variable(std::istream &in, std::string &pre_body)
{
    //cout << "to_variable" << endl;
    int i;
    char c;
    auto it_end = pre_body.end();
    while (it_end != pre_body.begin()) {
        if(isalnum(*it_end) || *it_end == '_') {
            ++it_end;
            break;
        }
        else
            --it_end;
    }
    auto it_begin = --it_end;
    while (it_begin != pre_body.begin()) {
        if(isalnum(*it_begin) || *it_begin == '_') {
            --it_begin;
        } else {
            ++it_begin;
            break;
        }
    }
    string name(it_begin, ++it_end);
    while((i = in.get()) != EOF) {
        c = (char)i;
        if (c == ';') {
            pre_body.push_back(c);
            string temp;
            getline(in, temp);
            in.unget();
            pre_body += temp; //如果}还有一段关于一段该函数的注释或其他
            break;
        } else if (c == '/') {
            pre_body += pass_illustrate(in);
        } else if (c == '"' || c == '\'') {
            pre_body += pass_string(in);
        } else {
            pre_body.push_back(c);
        }
    }
    g_variable_map.insert(make_pair(name, pre_body));
    return;
}

/********************************
*未定义代码
*录入这一行
* @todo bug:最后一个是},没有换行，loop
********************************/
void Parse_c::to_undentified(std::istream& in, std::string& pre_body)
{
//    cout << "to undentified() is running" << endl;
//    cout<< "un_identified:" << (char)in.peek() << endl;
    string s;
    getline(in, s);
    in.unget(); //loop bug
    pre_body += s;
    un_identified.push_back(pre_body);
}








