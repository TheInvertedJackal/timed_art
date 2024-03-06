#ifndef IMAGE_HOLDER_H
#define IMAGE_HOLDER_H

#include <SDL2/SDL_image.h>
#include <string>
#include <vector>

const int MAX_IMAGES = 5000;
const double IMG_RATIO_W = .95, IMG_RATIO_H = .9;

class ImageHolder{
    public:
        //Takes in the path of the image.
        ImageHolder(SDL_Renderer*, std::string*, double, double);
        //Gets the image texture
        SDL_Texture * getTexture();
        //Deletes the image texture
        void destoryTexture();
        void normalize(SDL_Rect*);
        void setWindowXAndY(double, double);
    private:
        std::string* _path;
        SDL_Renderer* _rend;
        SDL_Texture * _texture;
        bool _img_in_use;
        double _screen_x, _screen_y;
        int _img_width, _img_height;
};

class ImagePool{
    public:
        ImagePool();
        void addImage(ImageHolder*);
        ImageHolder* getFreshImage();
        void deletePool();
    private:
        void slotUsableImage(ImageHolder*);
        ImageHolder ** _useable_images = new ImageHolder*[MAX_IMAGES];
        ImageHolder ** _used_images = new ImageHolder*[MAX_IMAGES];
        int _useable_count;
        int _used_count;
        int _used_pointer;
        int _used_end;
        int _total_count;
};

#endif