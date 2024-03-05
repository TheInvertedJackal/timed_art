#ifndef ART_SESSION_H
#define ART_SESSION_H

#include <string>
#include <image_holder.h>

const int MAX_DEPTH = 5; //The max amount of dirs the file will go into.

class ArtSession{
    public:
        ArtSession(std::string, size_t duration = 60, size_t count = 0);
        bool isInfinite();
        size_t getTotalImgCount() const;
        std::string**getImages() const;
        //This deletes the pictures too
        void destroySession();
    private:
        void findPictures(std::string, int = 0);
        std::string** _imgs_paths = new std::string * [MAX_IMAGES];
        size_t _duration;
        size_t _count;
        size_t _img_count;
};

#endif