#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.hpp"
#include "../utilities/RGBColor.hpp"
#include "../world/ViewPlane.hpp"

#include <algorithm>

// constr: blank hres x vres
Image::Image(int hres, int vres) : hres(hres), vres(vres) {
    colors = new RGBColor*[hres]; //arr of col ptrs
    for (int i = 0; i < hres; i++) {
        colors[i] = new RGBColor[vres]; 
    }
}

Image::Image(const ViewPlane &vp) : Image(vp.hres, vp.vres) {}

// free arr
Image::~Image() {
    for (int i = 0; i < hres; i++) {
        delete[] colors[i];
    }
    delete[] colors;
}

// set pixel color at x, y
void Image::set_pixel(int x, int y, const RGBColor& color) {
    colors[x][y] = color;
}

// write to PNG file
void Image::write_png(std::string path) const {
    //packing pixels into arr
    // 3 bytes per pix
    unsigned char* data = new unsigned char[hres * vres * 3];

    for (int y = 0; y < vres; y++) {
        for (int x = 0; x < hres; x++) {
            int index = (y * hres + x) * 3;

            data[index] = (unsigned char)(colors[x][y].r * 255.0f);
            data[index + 1] = (unsigned char)(colors[x][y].g * 255.0f);
            data[index + 2] = (unsigned char)(colors[x][y].b * 255.0f);
        }
    }

    //bytes per row
    stbi_write_png(path.c_str(), hres, vres, 3, data, hres * 3);
    delete[] data;
}