#define MAKE_CASE(glType, storeType, enumType, glFunc)                                                                \
    case glType:                                                                                                      \
        for (int s = 0; s < size; s++)                                                                                \
        {                                                                                                             \
            if (s > 0)                                                                                                \
            {                                                                                                         \
                nameStr = nameStr.substr(0, nameStr.find_last_of("["));                                               \
                nameStr += "[" + std::to_string(s) + "]";                                                             \
            }                                                                                                         \
            auto loc = glGetUniformLocation(program, nameStr.data());                                                 \
            if (loc != GL_INVALID_INDEX)                                                                              \
            {                                                                                                         \
                storeType val;                                                                                        \
                glFunc(program, loc, &val);                                                                           \
                m_parameters.emplace(nameStr, new ShaderParameterType<storeType>(val, loc, ParameterType::enumType)); \
            }                                                                                                         \
        }                                                                                                             \
        break;

#define MAKE_CASE_VM(glType, storeType, enumType, glFunc)                                                             \
    case glType:                                                                                                      \
        for (int s = 0; s < size; s++)                                                                                \
        {                                                                                                             \
            if (s > 0)                                                                                                \
            {                                                                                                         \
                nameStr = nameStr.substr(0, nameStr.find_last_of("["));                                               \
                nameStr += "[" + std::to_string(s) + "]";                                                             \
            }                                                                                                         \
            auto loc = glGetUniformLocation(program, nameStr.data());                                                 \
            if (loc != GL_INVALID_INDEX)                                                                              \
            {                                                                                                         \
                storeType val;                                                                                        \
                glFunc(program, loc, glm::value_ptr(val));                                                            \
                m_parameters.emplace(nameStr, new ShaderParameterType<storeType>(val, loc, ParameterType::enumType)); \
            }                                                                                                         \
        }                                                                                                             \
        break;