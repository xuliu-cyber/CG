#ifndef PLANE
#define PLANE
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>
#include<math.h>
using namespace std;

class Plane {
    public:
    vector<float> planeVertices;
    unsigned int VAO, VBO;
    
    Plane(bool normal=false){
        if(normal){
            planeVertices={
                -1.0, -1.0, 0.0,0.0, -1.0, 0.0,
                1.0, -1.0, 0.0,0.0, -1.0, 0.0,
                1.0,1.0,0.0,0.0, -1.0, 0.0,
                -1.0, -1.0, 0.0,0.0, -1.0, 0.0,
                1.0,1.0,0.0,0.0, -1.0, 0.0,
                -1.0,1.0,0.0,0.0, -1.0, 0.0,
            };
        }
        else{
            planeVertices={
                -1.0, -1.0, 0.0,
                1.0, -1.0, 0.0,
                1.0,1.0,0.0,
                -1.0, -1.0, 0.0,
                1.0,1.0,0.0,
                -1.0,1.0,0.0
            };
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        //生成并绑定球体的VAO和VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //将顶点数据绑定至当前默认的缓冲中
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);

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
    }
};

#endif