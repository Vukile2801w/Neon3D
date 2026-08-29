#ifndef NEON_MESH
#define NEON_MESH

#include <filesystem>
#include <memory>

#include "Assets/AssetLoader.hpp"

#include "Rendering/Shader.hpp"

#include "Mesh/VertexBuffer.hpp"
#include "Mesh/IndexBuffer.hpp"
#include "Mesh/VertexArray.hpp"
#include "Mesh/MeshParser.hpp"
#include "Mesh/MeshData.hpp"

#include "Ref.hpp"

namespace Neon
{
    class Mesh
    {
    public:
        Mesh(
            const void *vertices,
            size_t vertexSize,
            const unsigned int *indices,
            size_t indexSize,
            const std::vector<BufferElement> &layout);

        ~Mesh() = default;

        void draw() const;

    private:
        VertexBuffer m_vertexBuffer;
        VertexArray m_vertexArray;
        IndexBuffer m_indexBuffer;

        size_t m_indexCount;
    };

    template <>
    struct AssetLoader<Mesh>
    {
        static Ref<Mesh> load(
            const std::filesystem::path &path)
        {
            MeshData data = MeshParser::parse(path);

            std::vector<BufferElement> layout = {
                {3, ShaderDataType::Float},
                {3, ShaderDataType::Float},
                {2, ShaderDataType::Float},
                {3, ShaderDataType::Float}};

            return std::make_shared<Mesh>(
                data.vertices.data(),
                data.vertices.size() * sizeof(MeshVertex),
                data.indices.data(),
                data.indices.size() * sizeof(unsigned int),
                layout);
        }
    };
}

#endif