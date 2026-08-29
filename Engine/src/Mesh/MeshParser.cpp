#include "Mesh/MeshParser.hpp"
#include "Logging.hpp"

#include "fast_obj/fast_obj.h"
#include "Logging.hpp"

namespace Neon
{

    MeshData MeshParser::parse(
        const std::filesystem::path &path)
    {
        fastObjMesh *obj = fast_obj_read(path.string().c_str());

        if (!obj)
        {
            Logging::Error(
                "Failed to load OBJ: " +
                path.string());

            return {};
        }

        if (obj->texcoord_count == 0)
        {
            Logging::Error(
                "We don't support OBJ files without texture coordinates yet: " +
                path.string());

            fast_obj_destroy(obj);
            return {};
        }

        if (obj->normal_count == 0)
        {
            Logging::Error(
                "We don't support OBJ files without normals yet: " +
                path.string());

            fast_obj_destroy(obj);
            return {};
        }

        if (obj->index_count == 0)
        {
            Logging::Error(
                "OBJ file has no faces yet: " +
                path.string());

            fast_obj_destroy(obj);
            return {};
        }

        MeshData data;

        data.vertices.reserve(obj->index_count);
        data.indices.reserve(obj->index_count);

        for (unsigned int i = 0; i < obj->index_count; ++i)
        {
            const fastObjIndex &index = obj->indices[i];

            if (index.t >= obj->texcoord_count)
            {
                Logging::Error(
                    "This OBJ has an invalid texture coordinate: " +
                    path.string());

                fast_obj_destroy(obj);
                return {};
            }

            if (index.n >= obj->normal_count)
            {
                Logging::Error(
                    "This OBJ has an invalid normal: " +
                    path.string());

                fast_obj_destroy(obj);
                return {};
            }

            MeshVertex vertex{};

            vertex.position = {
                obj->positions[index.p * 3 + 0],
                obj->positions[index.p * 3 + 1],
                obj->positions[index.p * 3 + 2]};

            vertex.texCoord = {
                obj->texcoords[index.t * 2 + 0],
                obj->texcoords[index.t * 2 + 1]};

            vertex.normal = {
                obj->normals[index.n * 3 + 0],
                obj->normals[index.n * 3 + 1],
                obj->normals[index.n * 3 + 2]};

            data.vertices.push_back(vertex);
            data.indices.push_back(i);
        }

        for (size_t i = 0; i < data.indices.size(); i += 3)
        {
            MeshVertex &v0 = data.vertices[data.indices[i + 0]];
            MeshVertex &v1 = data.vertices[data.indices[i + 1]];
            MeshVertex &v2 = data.vertices[data.indices[i + 2]];

            glm::vec3 edge1 =
                v1.position - v0.position;

            glm::vec3 edge2 =
                v2.position - v0.position;

            glm::vec2 deltaUV1 =
                v1.texCoord - v0.texCoord;

            glm::vec2 deltaUV2 =
                v2.texCoord - v0.texCoord;

            float determinant =
                deltaUV1.x * deltaUV2.y -
                deltaUV2.x * deltaUV1.y;

            if (std::abs(determinant) < 0.000001f)
                continue;

            float f = 1.0f / determinant;

            glm::vec3 tangent =
                f * (edge1 * deltaUV2.y -
                     edge2 * deltaUV1.y);

            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }

        for (MeshVertex &vertex : data.vertices)
        {
            glm::vec3 tangent =
                vertex.tangent -
                vertex.normal *
                    glm::dot(vertex.normal, vertex.tangent);

            if (glm::length(tangent) > 0.000001f)
            {
                vertex.tangent = glm::normalize(tangent);
            }
            else
            {
                glm::vec3 reference =
                    std::abs(vertex.normal.y) < 0.999f
                        ? glm::vec3(0.0f, 1.0f, 0.0f)
                        : glm::vec3(1.0f, 0.0f, 0.0f);

                vertex.tangent =
                    glm::normalize(
                        glm::cross(reference, vertex.normal));
            }
        }

        fast_obj_destroy(obj);

        return data;
    }
}