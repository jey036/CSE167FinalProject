#include "OBJObject.hpp"
#include "Window.hpp"

OBJObject::OBJObject(const char *filepath, const char *texturepath)
{
    toWorld = glm::mat4(1.0f);
    light_pos = glm::vec3(0.0, 0.0, 1.0);
    cam_pos = glm::vec3(0.0f, 1.0f, 20.0f);
    
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &TBO);
    
    parse(filepath);
    
    // bind the vertex array object then bind and set vertex buffer and attribute pointer
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textures.size(), &textures[0], GL_STATIC_DRAW);
    // TexCoord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_TRUE,sizeof(glm::vec2),(GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // load and create texture
    glGenTextures(1, &objTexture);
    glBindTexture(GL_TEXTURE_2D, objTexture);
    
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load, create texture and generate mipmaps
    int width,height;
    unsigned char* image;
    
    image = loadPPM(texturepath, width, height);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

OBJObject::~OBJObject() {
    // deallocate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void OBJObject::parse(const char *filepath)
{
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    
    FILE* fp;       // file pointer
    unsigned int fx,fy,fz;   // face indices
    unsigned int ex,ey,ez;   // face indices
    float x,y,z;    // vertex coordinates
    float r,g,b;    // vertex color
    float xt,yt; // vertex normal
    float minX = INFINITY, minY = INFINITY, minZ = INFINITY;   // min vertex
    float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;   // max vertex
    float avgX,avgY,avgZ;   // avg vector
    int c1,c2;      // characters read from file
    
    
    
    // open file
    fp = fopen(filepath,"rb");
    
    // if file can't be found or is corrupt
    if (fp == NULL) {
        std::cerr << "error loading file" << std::endl;
        exit(-1);
    }
    
    // load data till end of the file
    while (!feof(fp)) {
        
        c1 = fgetc(fp);
        
        if (c1 != 'v' && c1 != 'f') {
            continue;
        }
        
        c2 = fgetc(fp);
        
        // read vertices and vetrex normals
        if (c1 == 'v') {
            if (c2 == ' ') {
                fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
                
                // check for min and max
                if (x < minX) {
                    minX = x;
                }
                if (y < minY) {
                    minY = y;
                }
                if (z < minZ) {
                    minZ = z;
                }
                if (x > maxX) {
                    maxX = x;
                }
                if (y > maxY) {
                    maxY = y;
                }
                if (z > maxZ) {
                    maxZ = z;
                }
                
                vertices.push_back(glm::vec3(x,y,z));
            }
            if (c2 == 't') {
                fscanf(fp, "%f %f", &xt, &yt);
                textures.push_back(glm::vec2(xt,yt));
            }
        }
        
        // read faces
        if (c1 == 'f') {
            // different format
            if (filepath[0] == 'a') {
                fscanf(fp, "%d/%d %d/%d %d/%d", &fx, &fx, &fy, &fy, &fz, &fz);
                indices.push_back(fx - 1);
                indices.push_back(fy - 1);
                indices.push_back(fz - 1);
            }
            if (filepath[0] == 'c' && filepath[1] == 'u') {
                fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &fx, &ex, &ex, &fy, &ey, &ey, &fz, &ez, &ez);
                indices.push_back(fx - 1);
                indices.push_back(fy - 1);
                indices.push_back(fz - 1);
            }
            else {
                fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &fx, &fx, &fx, &fy, &fy, &fy, &fz, &fz, &fz);
                indices.push_back(fx - 1);
                indices.push_back(fy - 1);
                indices.push_back(fz - 1);
            }
        }
    }
    
    // get average vector
    avgX = (minX + maxX) / 2;
    avgY = (minY + maxY) / 2;
    avgZ = (minZ + maxZ) / 2;
    
    glm::vec3 center = glm::vec3(avgX,avgY,avgZ);
    
    // translate each vertex individually to new center origin
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] -= center;
    }
    
    // find longest dimension of model
    float dimX,dimY,dimZ;
    dimX = maxX - minX;
    dimY = maxY - minY;
    dimZ = maxZ - minZ;
    
    // scale all vertices using the largest dimension, uniform scale
    if (dimX >= dimY && dimX >= dimZ) {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i] = glm::vec3(vertices[i].x/dimX,vertices[i].y/dimX,vertices[i].z/dimX);
        }
    }
    else if (dimY >= dimX && dimY >= dimZ) {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i] = glm::vec3(vertices[i].x/dimY,vertices[i].y/dimY,vertices[i].z/dimY);
        }
    }
    else if (dimZ >= dimX && dimZ >= dimY) {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i] = glm::vec3(vertices[i].x/dimZ,vertices[i].y/dimZ,vertices[i].z/dimZ);
        }
    }
    
    minX = INFINITY, minY = INFINITY, minZ = INFINITY;   // min vertex
    maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;   // max vertex
    
    glm::vec3 minVertX, minVertY, minVertZ;
    glm::vec3 maxVertX, maxVertY, maxVertZ;
    
    // search for bounding box dimension
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].x < minX) {
            minX = vertices[i].x;
            minVertX = vertices[i];
        }
        if (vertices[i].y < minY) {
            minY = vertices[i].y;
            minVertY = vertices[i];
        }
        if (vertices[i].z < minZ) {
            minZ = vertices[i].z;
            minVertZ = vertices[i];
        }
        if (vertices[i].x > maxX) {
            maxX = vertices[i].x;
            maxVertX = vertices[i];
        }
        if (vertices[i].y > maxY) {
            maxY = vertices[i].y;
            maxVertY = vertices[i];
        }
        if (vertices[i].z > maxZ) {
            maxZ = vertices[i].z;
            maxVertZ = vertices[i];
        }
    }
    
    boxVertices.push_back(maxVertX);
    boxVertices.push_back(minVertX);
    boxVertices.push_back(maxVertY);
    boxVertices.push_back(minVertY);
    boxVertices.push_back(maxVertZ);
    boxVertices.push_back(minVertZ);
    
    // bounding box
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    //boxVertices.push_back(glm::vec3(minX,minY,maxZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    //boxVertices.push_back(glm::vec3(maxX,minY,maxZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    //boxVertices.push_back(glm::vec3(maxX,maxY,maxZ));
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    //boxVertices.push_back(glm::vec3(minX,maxY,maxZ));
    
    /* */
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    //boxVertices.push_back(glm::vec3(minX,minY,minZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    //boxVertices.push_back(glm::vec3(maxX,minY,minZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    //boxVertices.push_back(glm::vec3(maxX,maxY,minZ));
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    //boxVertices.push_back(glm::vec3(minX,maxY,minZ));
    
    /* */
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    
    // close file
    fclose(fp);
}

void OBJObject::draw(GLuint shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader,"model"), 1, GL_FALSE, &toWorld[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader,"view"), 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"), 1, GL_FALSE, &Window::P[0][0]);
    
    glBindTexture(GL_TEXTURE_2D, objTexture);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OBJObject::drawWater(GLuint shader, GLuint box) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    
    glm::vec3 eye = glm::transpose(glm::mat3(Window::V))*glm::vec3(Window::V[3][0], Window::V[3][1], Window::V[3][2]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"),1, &eye[0]);
    
    //glUniform1i(glGetUniformLocation(shaderProgram, "Noise3"), 1);
    
    // glBindTexture(GL_TEXTURE_2D, objTexture);
    // glUniform1i(glGetUniformLocation(shaderProgram, "dudvMap"), 0);
    // glEnable(GL_TEXTURE_2D);
    
    // glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, box);
    // glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
    // glEnable(GL_TEXTURE_2D);
    
    //glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, objTexture);
    //
    
    //glDrawArrays(GL_TRIANGLES, 0, indices.size());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/** Load a ppm file from disk.
 @input filename The location of the PPM file.  If the file is not found, an error message
 will be printed and this function will return 0
 @input width This will be modified to contain the width of the loaded image, or 0 if file not found
 @input height This will be modified to contain the height of the loaded image, or 0 if file not found
 @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
 **/
unsigned char* OBJObject::loadPPM(const char* filename, int& width, int& height)
{
    const int BUFSIZE = 128;
    FILE* fp;
    unsigned int read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;
    
    if ( (fp=fopen(filename, "rb")) == NULL)
    {
        std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
        width = 0;
        height = 0;
        return NULL;
    }
    
    // Read magic number:
    retval_fgets = fgets(buf[0], BUFSIZE, fp);
    
    // Read width and height:
    do
    {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    retval_sscanf=sscanf(buf[0], "%s %s", buf[1], buf[2]);
    width  = atoi(buf[1]);
    height = atoi(buf[2]);
    
    // Read maxval:
    do
    {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    
    // Read image data:
    rawData = new unsigned char[width * height * 3];
    read = fread(rawData, width * height * 3, 1, fp);
    fclose(fp);
    if (read != 1)
    {
        std::cerr << "error parsing ppm file, incomplete data" << std::endl;
        delete[] rawData;
        width = 0;
        height = 0;
        return NULL;
    }
    
    return rawData;
}
