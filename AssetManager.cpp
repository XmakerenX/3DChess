#include "AssetManager.h"

//-----------------------------------------------------------------------------
// Name : AssetManager (constructor)
//-----------------------------------------------------------------------------
AssetManager::AssetManager()
{

}

//-----------------------------------------------------------------------------
// Name : AssetManager (destructor)
//-----------------------------------------------------------------------------
AssetManager::~AssetManager()
{

}

//-----------------------------------------------------------------------------
// Name : getTexture
//-----------------------------------------------------------------------------
GLuint AssetManager::getTexture(const std::string& filePath)
{
    // check if the texture is already loaded
    if (textureCache_.count(filePath) != 0)
    {
        // return textrue id(name)
        return textureCache_[filePath];
    }
    // else load the textrue
    else
    {
        //return loadPng(filePath);
        return loadBMP(filePath);
    }


}

//-----------------------------------------------------------------------------
// Name : loadPng
//-----------------------------------------------------------------------------
GLuint AssetManager::loadPng(const std::string &filePath)
{
    int number_of_passes = 0;
    int format = 0;

    // header for testing if it is a png
    png_byte header[8];

    // open file as binary
    FILE* fp = fopen(filePath.c_str(), "rb");
    if (!fp)
    {
        std::cout << "Failed to open " << filePath << "\n";
        return 0;
    }

    // read header
    fread(header, 1, 8, fp);

    // test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png)
    {
        fclose(fp);
        std::cout<<"The given file is not a valid png file\n";
        return 0;
    }

    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,nullptr,nullptr);
    if (!png_ptr)
    {
        fclose(fp);
        std::cout << "Failed to create png struct\n";
        return 0;
    }

    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp) nullptr, (png_infopp) nullptr);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)nullptr);
        fclose(fp);
        std::cout << "Failed to create png info struct\n";
        return 0;
    }

    // png error stuff, not sure libpng man suggests this.
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        std::cout << "png error stuff\n";
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read al the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 width, height;

    number_of_passes = png_set_interlace_handling(png_ptr);

    //get info about png
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

    // set the correct image format to laod
    if (color_type == PNG_COLOR_TYPE_RGB)
        format = GL_RGB;
    if (color_type == PNG_COLOR_TYPE_RGBA)
        format = GL_RGBA;

    // Update width and height based on the png info
    //width = twidth;
    //height = theight;

    // Update the png info struct
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes
    int rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte* image_data = new png_byte[rowBytes * height];
    if (!image_data)
    {
        //clean up memory and close stuff
        std::cout << "Fail to allocate buffer for image\n";
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for the pointing to image_data for reading the png with libpng
    png_bytep* row_pointers = new png_bytep[height];
    if (!row_pointers)
    {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] image_data;
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (int i = 0; i < height; i++)
        row_pointers[height - 1 - i] = image_data + i * rowBytes;

    // read the png into image data through row_pointers
        png_read_image(png_ptr, row_pointers);


    // now generate the OpenGL texture
    GLuint textureID = createTexture(width, height, format, image_data);;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
//                 0, format, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] image_data;
    delete[] row_pointers;
    fclose(fp);

    textureCache_.insert(std::pair<std::string, GLuint>(filePath,textureID));
    return textureID;
    return 0;
}

//-----------------------------------------------------------------------------
// Name : loadBMP
//-----------------------------------------------------------------------------
GLuint AssetManager::loadBMP(const std::string &filePath)
{
    // Each BMP file begins by a 54-bytes header
    unsigned char header[54];
    // Position in the file where the actual data begins
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    unsigned char* data;

    // open file
    std::ifstream file;
    file.open(filePath, std::ifstream::binary);
    if (!file.is_open())
    {
        std::cout << "Failed to open "<< filePath << "\n";
        return 0;
    }

    file.read((char*)header, 54);
    if (!file)
    {
        file.close();
        std::cout << "Not a correct BMP file\n";
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M' )
    {
        file.close();
        std::cout << "Not a correct BMP file\n";
        return 0;
    }

    if (header[28] != 24 && header[28] != 32)
    {
        file.close();
        std::cout << "Invalid file format. only 24 or 32 are supported \n";
        return 0;
    }

    // Read ints from the byte array
    dataPos   =   *(int*)&(header[0x0A]);
    imageSize =   *(int*)&(header[0x22]);
    width     =   *(int*)&(header[0x12]);
    height    =   *(int*)&(header[0x16]);
    unsigned int bitsPerPixel = header[28];
    //width     =   header[18] + (header[19] << 8);
    //height    =   header[22] + (header[23] << 8);
    //dataPos   = header[10] + (header[11] << 8);
    imageSize = ((width * bitsPerPixel + 31) /32)* 4 * height;


    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = width * height * 3; // one byte per color RGB
    if (dataPos == 0)
        dataPos = 54;

    // create a buffer
    data = new unsigned char[imageSize];

    if (!data)
    {
        //clean up memory and close stuff
        std::cout << "Fail to allocate buffer for image\n";
        file.close();
        return 0;
    }

    // read the actual data from the file into the buffer
    file.seekg(dataPos, std::ifstream::beg);
    file.read((char*)data, imageSize);

    GLuint textureID = createTexture(width, height, GL_BGR, data);

    //clean up memory and close stuff
    file.close();
    delete[] data;

    textureCache_.insert(std::pair<std::string, GLuint>(filePath,textureID));

    return textureID;
}

//-----------------------------------------------------------------------------
// Name : createTexture
//-----------------------------------------------------------------------------
GLuint AssetManager::createTexture(GLsizei width, GLsizei height, GLenum format, unsigned char *data)
{
    // generate the OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                 0, format, GL_UNSIGNED_BYTE, (GLvoid*)data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return textureID;
}
