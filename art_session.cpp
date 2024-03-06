#include <art_session.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;
namespace fs = std::filesystem;

const int ENDINGS_COUNT = 3; //The number of aceptable endings
const int MAX_LOWER = 10; //The number of chars to make lowercase on the end
const string pictureEndings[ENDINGS_COUNT] = {".png", ".jpg", ".webmp"};

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    bool isValid = true;
    size_t valueLength = value.length() - 1;
    for (size_t i = ending.length() - 1; i > 0 && isValid; --i)
    {
        isValid = ending[i] == value[valueLength];
        --valueLength;
    }
    return isValid;
}

ArtSession::ArtSession(string dir, size_t dur, size_t count): _duration{dur}, _count{count}, _img_count{0}{
    findPictures(dir);
}

bool ArtSession::isInfinite(){
    return _count == 0;
}

string ** ArtSession::getImages() const{
    return _imgs_paths;
}

size_t ArtSession::getTotalImgCount() const {
    return _img_count;
}

size_t ArtSession::getDuration() const {
    return _duration;
}

size_t ArtSession::getCount() const {
    return _count;
}

void ArtSession::findPictures(string dir, int depth){
    ++depth;
    if(depth > MAX_DEPTH || _count >= MAX_IMAGES) return;
    fs::path ** sub_dirs = new fs::path * [MAX_SUB_DIRS];
    size_t sub_dir_count = 0;
    for (const fs::path & entry : fs::directory_iterator(dir)){
        fs::path abs_file = fs::absolute(entry);
        string temp = abs_file.string();
        if(fs::is_directory(abs_file) && sub_dir_count < MAX_SUB_DIRS){
            sub_dirs[sub_dir_count] = new fs::path(abs_file);
            ++sub_dir_count;
        } else {
            bool valid = false;
            for (size_t i = temp.length() - 1; i > temp.length() - MAX_LOWER; --i)
            {
                temp[i] = tolower(temp[i]);
            }
            for (size_t i = 0; i < ENDINGS_COUNT && !valid; i++)
            {
                valid = ends_with(temp, pictureEndings[i]);
            }
            if(valid && _img_count < MAX_IMAGES){
                string * temp = new string(abs_file.string());
                _imgs_paths[_img_count] = temp;
                ++_img_count;
            }
        }
    }
    for (size_t i = 0; i < sub_dir_count; i++)
    {
        findPictures(sub_dirs[i]->string(), depth);
        delete sub_dirs[i];
    }
    delete[] sub_dirs;
}

void ArtSession::destroySession() {
    for (size_t i = 0; i < _count; i++){
        delete _imgs_paths[i];
    }
    delete[] _imgs_paths;
}

