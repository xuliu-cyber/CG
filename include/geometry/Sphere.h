#ifndef SPHERE
#define SPHERE
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>
#include<math.h>
using namespace std;

class Sphere {
    public:
    int Y_SEGMENTS ;
    int X_SEGMENTS ;
    int indeces_size;
    const float PI= 3.14159265358979323846f;
    vector<float> sphereVertices;
	vector<int> sphereIndices;
    unsigned int VAO, VBO, EBO;
    
    Sphere(int x_segs=50, int y_segs=50, bool normal=false){
        X_SEGMENTS=x_segs;
        Y_SEGMENTS=y_segs;
        indeces_size=X_SEGMENTS*Y_SEGMENTS*6;
        for (int y=0;y<=Y_SEGMENTS;y++)
        {
            for (int x=0;x<=X_SEGMENTS;x++)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos =cos(xSegment * 2.0f * PI) *sin(ySegment * PI);
                float yPos =cos(ySegment * PI);
                float zPos =sin(xSegment * 2.0f * PI) *sin(ySegment * PI);
                sphereVertices.push_back(xPos);
                sphereVertices.push_back(yPos);
                sphereVertices.push_back(zPos);
                if(normal){
                    sphereVertices.push_back(xPos);
                    sphereVertices.push_back(yPos);
                    sphereVertices.push_back(zPos);
                }
            }
        }

        //生成球的Indices
        for (int i=0;i<Y_SEGMENTS;i++)
        {
            for (int j=0;j<X_SEGMENTS;j++)
            {
                sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
                sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
                sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j+1);
                sphereIndices.push_back(i* (X_SEGMENTS + 1) + j);
                sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
                sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
            }
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        //生成并绑定球体的VAO和VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //将顶点数据绑定至当前默认的缓冲中
        glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
        //设置顶点属性指针
        if(normal){
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        else{
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        
    }

    void dispose(){
        glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
	    glDeleteBuffers(1, &EBO);
    }
};

#endif