#pragma once
#include "Texture.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
namespace KuangyeEngine
{
namespace Graphics
{
enum class ParameterType
{
    Vec2,
    Vec3,
    Vec4,
    Float,
    Double,
    Int,
    UInt,
    Bool,
    Mat2,
    Mat3,
    Mat4,
    Sampler2D
};

struct TextureUnitBinding
{
    GLint texUnit;
    const Texture *texture;
    TextureUnitBinding(const GLint &texUnit, const Texture *texture) : texUnit(texUnit), texture(texture) {}
};

class ShaderParameter
{
    friend class KuangyeEngine::GameEditor;

public:
    ShaderParameter(){};
    ShaderParameter(const ShaderParameter &);
    ShaderParameter(ShaderParameter &&);
    ~ShaderParameter(){};

    ShaderParameter &operator=(const ShaderParameter &);
    ShaderParameter &operator=(ShaderParameter &&);

    template <typename T>
    void AddParameter(const std::string &name, const T &val);

    template <typename T>
    bool SetParameter(const std::string &name, const T &val);

    template <typename T>
    T *GetParameter(const std::string &name);

    void UpdateParameters(Shader *shader);

    void Use();

    /**
     * @brief change the current textures using the default naming system(see implementation)
     * 
     * @param newTextures the textures to be moved
     */
    void SetTextures(const std::vector<Texture *> &newTextures);

protected:
    class ShaderParameterBase
    {
    public:
        ShaderParameterBase(const unsigned int &loc, const ParameterType &type);
        ShaderParameterBase(const ShaderParameterBase &) = default;

        ShaderParameterBase &operator=(const ShaderParameterBase &) = default;
        ~ShaderParameterBase(){};

        virtual void Set(void *data) = 0;
        virtual void Use() = 0;
        virtual void *Get() = 0;
        ParameterType GetType();
        virtual std::unique_ptr<ShaderParameterBase> Clone() = 0;

        void UpdateLocation(const unsigned int &loc);

    protected:
        unsigned int m_loc;
        ParameterType m_type;
    };

    template <typename T>
    class ShaderParameterType : public ShaderParameterBase
    {
    public:
        ShaderParameterType();
        ~ShaderParameterType();
        ShaderParameterType(const T &val, const unsigned int &loc, const ParameterType &type);

        ShaderParameterType(const ShaderParameterType &) = default;
        ShaderParameterType &operator=(const ShaderParameterType &) = default;
        void Set(const T &val);

        virtual void Set(void *data) override;
        virtual void Use() override;
        virtual void *Get() override;
        virtual std::unique_ptr<ShaderParameterBase> Clone() override;

    private:
        T m_val;
    };

    std::unordered_map<std::string, std::unique_ptr<ShaderParameterBase>> m_parameters;
};

template <typename T>
void ShaderParameter::AddParameter(const std::string &name, const T &val)
{
    m_parameters.emplace(name, new ShaderParameterType<T>(name, val));
}

template <typename T>
bool ShaderParameter::SetParameter(const std::string &name, const T &val)
{
    auto iter = m_parameters.find(name);
    if (iter == m_parameters.end())
        return false;
    iter->second->Set((void *)&val);
    return true;
}

template <typename T>
T *ShaderParameter::GetParameter(const std::string &name)
{
    auto iter = m_parameters.find(name);
    if (iter != m_parameters.end())
        return static_cast<T *>(iter->second->Get());
    return nullptr;
}

template <typename T>
ShaderParameter::ShaderParameterType<T>::ShaderParameterType() : ShaderParameterBase(""), m_val(0)
{
}

template <typename T>
ShaderParameter::ShaderParameterType<T>::~ShaderParameterType(){};

template <typename T>
ShaderParameter::ShaderParameterType<T>::ShaderParameterType(const T &val, const unsigned int &loc, const ParameterType &type) : ShaderParameterBase(loc, type), m_val(val)
{
}

template <typename T>
void ShaderParameter::ShaderParameterType<T>::Set(void *data)
{
    m_val = *static_cast<T *>(data);
}

template <typename T>
void ShaderParameter::ShaderParameterType<T>::Use()
{
    Shader::setUniform(m_loc, m_val);
}

template <typename T>
void *ShaderParameter::ShaderParameterType<T>::Get()
{
    return &m_val;
}
template <typename T>
void ShaderParameter::ShaderParameterType<T>::Set(const T &val)
{
    m_val = val;
}

template <typename T>
std::unique_ptr<ShaderParameter::ShaderParameterBase> ShaderParameter::ShaderParameterType<T>::Clone()
{

    return std::make_unique<ShaderParameter::ShaderParameterType<T>>(*this);
}
} // namespace Graphics
} // namespace KuangyeEngine