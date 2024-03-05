#include <image_holder.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>

using namespace std;

ImageHolder::ImageHolder(SDL_Renderer* rend,string path, double x, double y): 
    _path{path},_rend{rend}, _img_in_use{false}, _x_fit{x}, _y_fit{y}{}

SDL_Texture * ImageHolder::getTexture(){
    SDL_Surface* sur = IMG_Load(_path.c_str());
    if(sur == NULL){
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(_rend, sur);
    if(tex == NULL){
        return nullptr;
    }
    SDL_FreeSurface(sur);
    _img_in_use = true;
    _texture = tex;
    return tex;
}

void ImageHolder::destoryTexture(){
    if(_img_in_use){
        _img_in_use = false;
        SDL_DestroyTexture(_texture);
    }
}

ImagePool::ImagePool():
     _useable_count{0}, _used_count{0}, _used_pointer{0}, _used_end{0}, _total_count{0}{}

void ImagePool::addImage(ImageHolder * new_img){
    if(_useable_count < MAX_IMAGES){
        slotUsableImage(new_img);
        ++_total_count;
    }
}

void ImagePool::slotUsableImage(ImageHolder * img){
    _useable_images[_useable_count] = img;
    ++_useable_count;
}

ImageHolder* ImagePool::getFreshImage(){
    if(_useable_count == 0){
        return nullptr;
    }

    int rv = rand() % _useable_count;
    ImageHolder * ret_val = _useable_images[rv];
    --_useable_count;
    if(rv != _useable_count){
        ImageHolder * end_value = _useable_images[_useable_count];
        _useable_images[rv] = end_value;
    }
    _used_images[_used_pointer] = ret_val;
    _used_pointer = (_used_pointer + 1) % MAX_IMAGES;
    ++_used_count;
    if(((double) _used_count) / _total_count >= .6 || _useable_count == 0){
        --_used_count;
        slotUsableImage(_used_images[_used_end]);
        _used_end = (_used_end + 1) % MAX_IMAGES;
    }
    return ret_val;
}

void ImagePool::deletePool(){
    
}