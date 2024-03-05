#include <art_session.h>
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

const int ENDINGS_COUNT = 3; //The number of aceptable endings
const string pictureEndings[ENDINGS_COUNT] = {".png", ".jpeg", ".webmp"};

ArtSession::ArtSession(string dir, size_t dur, size_t count): _duration{dur}, _count{0}, _img_count{0}{
    findPictures(dir);
}

bool ArtSession::isInfinite(){
    return _count == 0;
}

string** ArtSession::getImages() const{
    return _imgs_paths;
}

size_t ArtSession::getTotalImgCount() const {
    return _img_count;
}

void ArtSession::findPictures(string dir, int depth){
    ++depth;
    if(depth > MAX_DEPTH) return;
    
    for (const fs::path & entry : fs::directory_iterator(dir))
        cout << fs::absolute(entry) << endl;
}

void ArtSession::destroySession() {
    for (size_t i = 0; i < _count; i++){
        delete _imgs_paths[i];
    }
    delete[] _imgs_paths;
}

