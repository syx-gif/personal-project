#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "util.hpp"




const std::string src_path = "data/input";
const std::string output = "data/raw_html/raw.txt";

typedef struct DocInfo{
    std::string title;   
    std::string content; 
    std::string url;     
}DocInfo_t;

//const &: 输入
//*: 输出
//&：输入输出
bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list);
bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results);
bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output);

int main()
{
    std::vector<std::string> files_list;
    
    if(!EnumFile(src_path, &files_list)){
        std::cerr << "enum file name error!" << std::endl;
        return 1;

    }
    
    std::vector<DocInfo_t> results;
    if(!ParseHtml(files_list, &results)){
        std::cerr << "parse html error" << std::endl;
        return 2;
    }

    
    if(!SaveHtml(results, output)){
        std::cerr << "sava html error" << std::endl;
        return 3;
    }

    return 0;
}

bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list)
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);

    
    if(!fs::exists(root_path)){
        std::cerr << src_path << " not exists" << std::endl;
        return false;
    }

   
    fs::recursive_directory_iterator end;
    for(fs::recursive_directory_iterator iter(root_path); iter != end; iter++){
      
        if(!fs::is_regular_file(*iter)){ 
            continue;
        }
        if(iter->path().extension() != ".html"){ 
            continue;
        }
        
        files_list->push_back(iter->path().string()); 
    }

    return true;
}

static bool ParseTitle(const std::string &file, std::string *title)
{
    std::size_t begin = file.find("<title>");
    if(begin == std::string::npos){
        return false;
    }
    std::size_t end = file.find("</title>");
    if(end == std::string::npos){
        return false;
    }

    begin += std::string("<title>").size();

    if(begin > end){
        return false;
    }
    *title = file.substr(begin, end - begin);
    return true;
}

static bool ParseContent(const std::string &file, std::string *content)
{
    
    enum status{
        LABLE,
        CONTENT
    };

    enum status s = LABLE;
    for( char c : file){
        switch(s){
            case LABLE:
                if(c == '>') s = CONTENT;
                break;
            case CONTENT:
                if(c == '<') s = LABLE;
                else {
                    
                    if(c == '\n') c = ' ';
                    content->push_back(c);
                }
                break;
            default:
                break;
        }
    }

    return true;
}

static bool ParseUrl(const std::string &file_path, std::string *url)
{
    std::string url_head = "https://www.boost.org/doc/libs/1_78_0/doc/html";
    std::string url_tail = file_path.substr(src_path.size());

    *url = url_head + url_tail;
    return true;
}

//for debug
static void ShowDoc( const DocInfo_t &doc)
{
    std::cout << "title: " << doc.title << std::endl;
    std::cout << "content: " << doc.content << std::endl;
    std::cout << "url: " << doc.url << std::endl;
}

bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results)
{
    for(const std::string &file : files_list){
      
        std::string result;
        if(!ns_util::FileUtil::ReadFile(file, &result)){
            continue;
        }
        DocInfo_t doc;
        
        if(!ParseTitle(result, &doc.title)){
            continue;
        }
       
        if(!ParseContent(result, &doc.content)){
            continue;
        }
       
        if(!ParseUrl(file, &doc.url)){
            continue;
        }

       
        results->push_back(std::move(doc)); 

        //for debug
        //ShowDoc(doc);
        //break;
    }
    return true;
}

bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output)
{
#define SEP '\3'
    
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if(!out.is_open()){
        std::cerr << "open " << output << " failed!" << std::endl;
        return false;
    }

    
    for(auto &item : results){
        std::string out_string;
        out_string = item.title;
        out_string += SEP;
        out_string += item.content;
        out_string += SEP;
        out_string += item.url;
        out_string += '\n';

        out.write(out_string.c_str(), out_string.size());
    }

    out.close();

    return true;
}
