#include "AssetManager.h"

#include<algorithm>

//-----------------------------------------------------------------------------
// Name : Group::addVertex
//-----------------------------------------------------------------------------
GLushort Group::addVertex(Model& model,int* v, int* t, int* n)
{
	glm::vec3 pos;
	glm::vec3 normal = {0.0f, 0.0f, 0.0f};
	glm::vec2 texCords = {0.0f, 0.0f};
		
	if (v != nullptr)
	{
		if (*v >= 0)
		{
			pos.x = model.verticesPos[3*(*v - 1) + 0];
			pos.y = model.verticesPos[3*(*v - 1) + 1];
			pos.z = model.verticesPos[3*(*v - 1) + 2];
		}
		else
		{
			pos.x = model.verticesPos[3*(*v + model.numVertices) + 0];
			pos.y = model.verticesPos[3*(*v + model.numVertices) + 1];
			pos.z = model.verticesPos[3*(*v + model.numVertices) + 2];
		}
	}
		
	if (n != nullptr)
	{
		if (*n >= 0)
		{
			normal.x = model.normals[3*(*n - 1) + 0];
			normal.y = model.normals[3*(*n - 1) + 1];
			normal.z = model.normals[3*(*n - 1) + 2];
		}
		else
		{
			normal.x = model.normals[3*(*n + model.numNormals) + 0];
			normal.y = model.normals[3*(*n + model.numNormals) + 1];
			normal.z = model.normals[3*(*n + model.numNormals) + 2];
		}
	}

	if (t != nullptr)
	{
		if (*t != 0)
		{
			texCords.x = model.texCords[2*(*t - 1) + 0];
			texCords.y = model.texCords[2*(*t - 1) + 1];
		}
		else
		{
			texCords.x = model.texCords[2*(*t + model.numTexCords) + 0];
			texCords.y = model.texCords[2*(*t + model.numTexCords) + 1];
		}
	}
		
	return addVertex(pos, normal, texCords);
}

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
    if (m_textureCache.count(filePath) != 0)
    {
        // return textrue id(name)
        return m_textureCache[filePath];
    }
    // else load the textrue
    else
    {
        std::string suffix;
        std::stringstream s(filePath);
        // gets the file name
        std::getline(s, suffix, '.');
        // gets the file suffix
        std::getline(s, suffix, '.');
        // load the texutre using the appropriate method
        if (suffix == "png")
            return loadPng(filePath);
        if (suffix == "jpg")
            return loadJPEG(filePath);
        if (suffix == "bmp")
            return loadBMP(filePath);

        std::cout << suffix << " is not a supported texture type\n";
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Name : loadPng
//-----------------------------------------------------------------------------
GLuint AssetManager::loadPng(const std::string &filePath)
{
    int number_of_passes = 0;
    unsigned int format = 0;

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

    // create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,nullptr,nullptr);
    if (!png_ptr)
    {
        fclose(fp);
        std::cout << "Failed to create png struct\n";
        return 0;
    }

    // create png info struct
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

    // set the correct image format
    if (color_type == PNG_COLOR_TYPE_RGB)
        format = GL_RGB;
    if (color_type == PNG_COLOR_TYPE_RGBA)
        format = GL_RGBA;

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
    GLuint textureID = createTexture(width, height, format, image_data);

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] image_data;
    delete[] row_pointers;
    fclose(fp);

    // cached the loaded texture
    m_textureCache.insert(std::pair<std::string, GLuint>(filePath,textureID));
    return textureID;
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

    // clean up memory and close stuff
    file.close();
    delete[] data;

    // cache the loaded texture
    m_textureCache.insert(std::pair<std::string, GLuint>(filePath,textureID));

    return textureID;
}

//-----------------------------------------------------------------------------
// Name : loadJPEG
//-----------------------------------------------------------------------------
GLuint AssetManager::loadJPEG(const std::string &filePath)
{
    unsigned int width, height;
    int channels; //  3 =>RGB   4 =>RGBA
    unsigned int format = 0;

    unsigned char * rowptr[1];    // pointer to an array
    unsigned char * jdata;        // data for the image
    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err;          //the error handler

    FILE* file = fopen(filePath.c_str(), "rb");  //open the file

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);   //fills info structure

    //if the jpeg file doesn't load
    if(!file)
    {
        std::cout << "Falied to open " << filePath << "\n";
        return 0;
    }

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);

    //TODO: check jpeg size before starting decompressing
    jpeg_start_decompress(&info);

    width = info.output_width;
    height = info.output_height;

    channels = info.num_components;
    // set texture type
    if (channels < 3)
    {
        std::cout << "jpegs with less than 3 channels are not supported\n";
        return 0;
    }

    format = GL_RGB;
    if(channels == 4) format = GL_RGBA;

    unsigned long data_size = width * height * info.output_components;
    unsigned int rowStride = channels * info.output_width;

    // read scanlines one at a time into jdata[] array
    jdata = new unsigned char[data_size];
    while (info.output_scanline < info.output_height)
    {

        // point rowptr to the current row to be filled with data
        // libjpeg loads from end to start to rowptr starts from the end of jdata
        //          start +         end        -  current line  - line(to get to the start of the line)
        rowptr[0] = jdata + rowStride * height - rowStride * info.output_scanline - rowStride;

        // load data to the current line
        jpeg_read_scanlines(&info, rowptr, 1);

    }

    jpeg_finish_decompress(&info);
    GLuint textureID = createTexture(width, height, format, jdata);

    // free resources
    jpeg_destroy_decompress(&info);
    fclose(file);
    delete[] jdata;

    // cache the loaded texture
    m_textureCache.insert(std::pair<std::string, GLuint>(filePath,textureID));

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
	
	if (textureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, (GLvoid*)data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
		std::cout << "Failed to generate a texture name\n";
	
	return textureID;
}

//-----------------------------------------------------------------------------
// Name : getMesh
//-----------------------------------------------------------------------------
Mesh* AssetManager::getMesh(const std::string& meshPath)
{
    // check if the texture is already loaded
    if (m_meshCache.count(meshPath) != 0)
    {
        // return textrue id(name)
        return &m_meshCache[meshPath];
    }
    // else load the textrue
    else
    {
        std::string suffix;
        std::stringstream s(meshPath);
        // gets the file name
        std::getline(s, suffix, '.');
        // gets the file suffix
        std::getline(s, suffix, '.');
        // load the texutre using the appropriate method
        if (suffix == "obj")
			return loadObjMesh(meshPath);

        std::cout << suffix << " is not a supported texture type\n";
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Name : loadObjMesh
//-----------------------------------------------------------------------------
Mesh* AssetManager::loadObjMesh(const std::string& meshPath)
{
	Model model(meshPath);
	std::ifstream in;
	
	in.open(meshPath);
	
	if (!in.is_open())
	{
		std::cout << "Failed to open "<< meshPath << "\n";
		return nullptr;
	}
	
	objFirstPass(in, model);
	
	// return to start of file
	in.clear();
	in.seekg(0, std::ios::beg);
	
	objSecondPass(in, model);
	
	// convert obj groups to subMeshes
	std::vector<SubMesh> subMeshes;
	std::vector<GLuint> meshMaterials;
	for (Group& g : model.groups)
	{
		// ignore empty groups
		if (g.vertices.size() != 0)
		{
			subMeshes.emplace_back(g.vertices, g.indices);
			meshMaterials.push_back(g.material);
		}
	}
	
	m_meshCache.insert(std::pair<std::string, Mesh>(meshPath, 
					Mesh(subMeshes, meshMaterials,std::vector<std::string>())) );
	//m_meshCache.emplace(meshPath, subMeshes, meshMaterials, std::vector<std::string>());
	
	return &m_meshCache[meshPath];
}

//-----------------------------------------------------------------------------
// Name : objFirstPass
//-----------------------------------------------------------------------------
void AssetManager::objFirstPass(std::ifstream& in ,Model& model)
{
	//char buf[128];
	std::string buf;
	Group* group = nullptr;
	bool readNextLine = true;
		
	int v, n, t;
	
	group = &model.addGroup("defualt");
	
	in >> buf;
	while (!in.eof())
	{
		switch(buf[0])
		{
			case '#':
			{
				// jump to the end of the line
				std::getline(in, buf);
				//in.getline(buf, 128);
			}break;
			
			case 'v':
			{
				switch(buf[1])
				{
					case '\0':
					{
						// jump to the end of the line
						std::getline(in, buf);
						//in.getline(buf, 128);
						model.numVertices++;
					}break;
					
					case 'n':
					{
						// jump to the end of the line
						std::getline(in, buf);
						//in.getline(buf, 128);
						model.numNormals++;
					}break;
					
					case 't':
					{
						// jump to the end of the line
						std::getline(in, buf);
						//in.getline(buf, 128);
						model.numTexCords++;
					}break;
					
					default:
						std::cout << "objFirstPass() - Unknown token\n";
						break;
				}
			}break;
			
			case 'm':
			{
				std::getline(in, buf);
				//in.getline(buf, 128);
				std::stringstream sStream(buf);
				sStream >> buf;
				sStream >> buf;
				model.matrialPath = buf;
				objReadMatrial(model, buf);
			}break;
			
			case 'u':
				std::getline(in, buf);
				//in.getline(buf, 128);
			break;
			
			case 'g':
			{
				std::getline(in, buf);
				//std::stringstream sStream(buf);
				//sStream >> buf;
				group = &model.addGroup(buf);
			}
			break;
			
			case 'f':
			{
				//std::string buf;
				v = n = t = 0;
				in >> buf;
				// can be one of v , v//n, v/t, v/t/n, x//x
				if (buf.find("//") != std::string::npos)
				{
					// v//n
					//TODO: add a function to read v and n
					std::stringstream s(buf);
					s >> v;
					s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
					s >> n;
					
					in >> buf;
					s = std::stringstream(buf);
					s >> v;
					s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
					s >> n;
					
					in >> buf;
					s = std::stringstream(buf);
					s >> v;
					s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
					s >> n;
					
					model.numTrinagles++;
					group->numTrinagles++;
					
					in >> buf;
					readNextLine = false;
					while (buf.find("//") != std::string::npos)
					{
						model.numTrinagles++;
						group->numTrinagles++;
						
						in >> buf;
					}
				}
				else if (std::count(buf.begin(), buf.end(), '/') == 3)
				{
					// v/t/
					in >> buf;
					std::replace(buf.begin(), buf.end(), '/', ' ');
					std::stringstream s(buf);
					s >> v;
					s >> t;
					s >> n;
					
					in >> buf;
					std::replace(buf.begin(), buf.end(), '/', ' ');
					s = std::stringstream(buf);
					s >> v;
					s >> t;
					s >> n;
					
					model.numTrinagles++;
					group->numTrinagles++;
					
					in >> buf;
					readNextLine = false;
					while ( std::count(buf.begin(), buf.end(), '/') > 0 )
					{
						model.numTrinagles++;
						group->numTrinagles++;
						
						in >> buf;
					}
				}
				else if (std::count(buf.begin(), buf.end(), '/') == 2)
				{
					// v/t
					in >> buf;
					std::replace(buf.begin(), buf.end(), '/', ' ');
					std::stringstream s(buf);
					s >> v;
					s >> t;
					
					in >> buf;
					std::replace(buf.begin(), buf.end(), '/', ' ');
					s = std::stringstream(buf);
					s >> v;
					s >> t;
					
					model.numTrinagles++;
					group->numTrinagles++;
					
					in >> buf;
					readNextLine = false;
					while ( std::count(buf.begin(), buf.end(), '/') > 0 )
					{
						model.numTrinagles++;
						group->numTrinagles++;
						
						in >> buf;
					}
				}
				else 
				{
					// v
					in >> v;
					
					model.numTrinagles++;
					group->numTrinagles++; 
					
					in >> buf;
					readNextLine = false;
					std::stringstream s(buf);
					s >> v;
					while (!s.fail())
					{
						model.numTrinagles++;
						group->numTrinagles++;
						
						in >> buf;
						s >> v;
					}
				}
			}break;
			
			default:
				std::getline(in, buf);
				//in.getline(buf, 128);
				break;

		}
		// next line was already read
		if (readNextLine)
			in >> buf;
		else
			readNextLine = true;
	}
	
	for (Group& g : model.groups)
	{
		//g.triangles = new GLuint[g.numTrinagles];
		g.numTrinagles = 0;
	}
}

//-----------------------------------------------------------------------------
// Name : objSecondPass
//-----------------------------------------------------------------------------
void AssetManager::objSecondPass(std::ifstream& in, Model& model)
{
	int v, n, t;
	GLuint numVertices;
	GLuint numNormals;
	GLuint numTexCords;
	GLuint numTrinagles;
	
	GLuint material;
	Group* group;
	
	std::string buf;
	
	group = &model.addGroup("defualt");
	
	numVertices = numNormals = numTexCords = 1;
	numTrinagles = 0;
	material = 0;
	
	model.verticesPos = new float[numVertices * 3];
	model.normals = new float[numNormals * 3];
	model.texCords = new float[numTexCords * 2];
	
	in >> buf;
	while (!in.eof())
	{
		switch(buf[0])
		{
			case '#':
			{
				// jump to the end of the line
				std::getline(in, buf);
				//in.getline(buf, 128);
			}break;
			
			case 'v':
			{
				switch(buf[1])
				{
					case '\0':
					{
						in >> model.verticesPos[3 * numVertices + 0];
						in >> model.verticesPos[3 * numVertices + 1];
						in >> model.verticesPos[3 * numVertices + 2];
						numVertices++;
					}break;
					
					case 'n':
					{
						in >> model.normals[3 * numNormals + 0];
						in >> model.normals[3 * numNormals + 1];
						in >> model.normals[3 * numNormals + 2];
						numNormals++;
					}break;
					
					case 't':
					{
						in >> model.texCords[2 * numTexCords + 0];
						in >> model.texCords[2 * numTexCords + 1];
						numTexCords++;
					}break;
				}
			}break;
			
			case 'u':
			{
				std::getline(in, buf);
				std::stringstream s(buf);
				
				s >> buf;
				s >> buf;
				group->material = material = model.materials[buf];
			}
			case 'g':
			{
				std::getline(in, buf);
				//in >> buf;
				group = &model.addGroup(buf);
				group->material = material;
			}break;
			case 'f':
			{
				std::string buffer;
				v = n = t = 0;
				
				in >> buffer;
				if (buffer.find("//") != std::string::npos)
				{
					// v//n
					// make the string easier to parse
					std::stringstream s;
					
					GLushort index;
					// merge all the face vertex data together and add to the vector
					for (int i = 0; i < 2; i ++)
					{
						// make the string easier to parse
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> n;
						
						index = group->addVertex(model,&v, nullptr, &n);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
					while (buffer.find("//") != std::string::npos)
					{
						// make the string easier to parse
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> n;
						
						int indicesSize = group->indices.size();
						group->indices.push_back(group->indices[indicesSize - 3]);
						group->indices.push_back(group->indices[indicesSize - 1]);
						
						index = group->addVertex(model, &v, nullptr, &n);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
				}
				else if (std::count(buffer.begin(), buffer.end(), '/') == 3)
				{
					// v/t/n
					GLushort index;
					
					for (int i = 0; i < 2; i++)
					{
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> t;
						s >> n;
						
						index = group->addVertex(model ,&v, &t, &n);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
					while (std::count(buffer.begin(), buffer.end(), '/') > 0)
					{
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> t;
						s >> n;
						
						int indicesSize = group->indices.size();
						group->indices.push_back(group->indices[indicesSize - 3]);
						group->indices.push_back(group->indices[indicesSize - 1]);
						
						index = group->addVertex(model, &v, &t, &n);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
				}
				else if (std::count(buffer.begin(), buffer.end(), '/') == 2)
				{
					// v/t
					GLushort index;
					
					for (int i = 0; i < 2; i++)
					{
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> t;
						
						index = group->addVertex(model, &v, &t, nullptr);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
					while (std::count(buffer.begin(), buffer.end(), '/') > 0)
					{
						std::replace(buffer.begin(), buffer.end(), '/', ' ');
						std::stringstream s(buffer);
						s >> v;
						s >> t;
						
						int indicesSize = group->indices.size();
						group->indices.push_back(group->indices[indicesSize - 3]);
						group->indices.push_back(group->indices[indicesSize - 1]);
						
						index = group->addVertex(model, &v, &t, nullptr);
						group->indices.push_back(index);
						
						in >> buffer;
					}
					
				}
				else
				{
					// v
					GLushort index;
					
					for (int i = 0; i < 2; i++)
					{
						in >> v;
						
						index = group->addVertex(model, &v, nullptr, nullptr);
						group->indices.push_back(index);
					}
					
					in >> v;
					while (!in.fail())
					{
						int indicesSize = group->indices.size();
						group->indices.push_back(group->indices[indicesSize - 3]);
						group->indices.push_back(group->indices[indicesSize - 1]);
						
						index = group->addVertex(model,&v, nullptr, nullptr);
						group->indices.push_back(index);
						
						in >> v;
					}
				}
			}break;
			
			default:
				std::getline(in, buf);
			break;
			
		}
		
		in >> buf;
	}
	
}

//-----------------------------------------------------------------------------
// Name : objReadMatrial
//-----------------------------------------------------------------------------
void AssetManager::objReadMatrial(Model& model, std::string matrialPath)
{
	std::ifstream in;
	std::string& meshPath = model.meshPath;
	std::string dir;
	char buf[128];
	
	GLuint numMaterials;
	Material curMaterial;
	std::string curMaterialName;
	
	std::size_t found = meshPath.find_last_of("/\\");
	if (found != std::string::npos)
		dir = meshPath.substr(0, found + 1);
	else
		dir = "";
	
	in.open(dir + matrialPath);
	
	if (!in.is_open())
	{
		std::cout << "objReadMatrial() failed: can't open material file "<< dir + matrialPath << "\n";
		return;
	}
	
	// first pass of the file
	numMaterials = 1;
	
	in >> buf;
	while (!in.eof())
	{
		switch(buf[0])
		{
			case '#':
				in.getline(buf, 128);
			break;
			case 'n':
				in.getline(buf, 128);
				numMaterials++;
			break;
			default:
				in.getline(buf, 128);
			break;
		}
		
		in >> buf;
	}
	
	in.clear();
	in.seekg(0, std::ios::beg);
	
	//model.materials = new Material[numMaterials];
	model.numMaterials = numMaterials;
	
	// second pass
	numMaterials = 0;
	
	in >> buf;
	while (!in.eof())
	{
		switch(buf[0])
		{
			case '#':
				in.getline(buf, 128);
			break;
			case 'n':
				GLuint materialIndex;

				if (numMaterials != 0)
				{
					// new material detected
					// save the pervious material
					materialIndex = getMaterialIndex(curMaterial);
					model.materials.insert(std::pair<std::string, GLuint>(curMaterialName, materialIndex));
					curMaterial = Material();
				}
				
				numMaterials++;
				
				in >> buf;
				curMaterialName = buf;
			break;
			case 'N':
			{
				float shininess;
				in >> curMaterial.power;
				curMaterial.power /= 1000.0;
				curMaterial.power *= 128.0;
			}
			break;
			
			case 'K':
				switch (buf[1])
				{
					case 'd':
						in >> curMaterial.diffuse.r;
						in >> curMaterial.diffuse.g;
						in >> curMaterial.diffuse.b;
						curMaterial.diffuse.a = 1.0f;
					break;
					case 's':
						in >> curMaterial.specular.r;
						in >> curMaterial.specular.g;
						in >> curMaterial.specular.b;
						curMaterial.specular.a = 1.0f;
					break;
					case 'a':
						in >> curMaterial.ambient.r;
						in >> curMaterial.ambient.g;
						in >> curMaterial.ambient.b;
						curMaterial.ambient.a = 1.0f;
					break;
					default:
						in.getline(buf, 128);
					break;
				}
		}
		in >> buf;
	}
}

//-----------------------------------------------------------------------------
// Name : getShader
//-----------------------------------------------------------------------------
Shader* AssetManager::getShader(const std::string& shaderPath)
{
	// check if the shader is loaded in the cache
    if (m_shaderCache.count(shaderPath) != 0)
    {
        // return textrue id(name)
        return m_shaderCache[shaderPath];
    }

    // no such shader in cache , adding a new one 
    std::string vertexShader = shaderPath + ".vs";
	std::string fragmentShader = shaderPath + ".frag";
    
	Shader* shader = new Shader(vertexShader.c_str(), fragmentShader.c_str());
	
	m_shaderCache.insert(std::pair<std::string, Shader*>(shaderPath,shader));
	
	return shader;
}

//-----------------------------------------------------------------------------
// Name : getMaterialIndex
//-----------------------------------------------------------------------------
int AssetManager::getMaterialIndex(const Material& mat)
{
	for (unsigned int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i] == mat)
			return i;
	}
	
	// no matching material found , adding a new one
	m_materials.push_back(mat);
	
	return m_materials.size() - 1;
}

//-----------------------------------------------------------------------------
// Name : getAttribute
//-----------------------------------------------------------------------------
int AssetManager::getAttribute(const std::string& texPath, const Material& mat,const std::string& shaderPath)
{
	unsigned int  matIndex = getMaterialIndex(mat);
	Attribute attrib = {texPath, matIndex, shaderPath};
	
	for (unsigned int i = 0; i < m_attributes.size(); i++)
	{
		if (attrib == m_attributes[i])
			return i;
	}
	
	// no matching Attribute found , adding a new one
	m_attributes.push_back(attrib);
	
	return m_attributes.size() - 1;
	
}

