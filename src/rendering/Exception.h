//
// Created by Jonas on 11/03/2021.
//

#ifndef MESHCORE_EXCEPTION_H
#define MESHCORE_EXCEPTION_H

#include <iostream>

#ifdef NDEBUG
#define GL_CALL(call) call
#else
#define GL_CALL(call)                                               \
while(glGetError() != GL_NO_ERROR){}                                \
    call;                                                           \
    while(GLenum error = glGetError()){                             \
        std::cout<< "[OpenGL Error] (" << error << "): " <<         \
        #call << " " << __FILE__ << ":" << __LINE__ << std::endl;   \
    } do {} while(0)
#endif

#endif //MESHCORE_EXCEPTION_H
