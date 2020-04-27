#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "model.h"

#include "obj_loader.h"

bool Model::load_obj_model(std::string filename) {
    // Initialize Loader
    objl::Loader Loader;

    // Load .obj File
    bool loadout = Loader.LoadFile(filename);

    // Check to see if it loaded
    if (!loadout) return false;

        for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++) {
            // Copy one of the loaded meshes to be our current mesh
            objl::Mesh curMesh = Loader.LoadedMeshes[i];

            // Print Mesh Name
            std::cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";

            // Print Vertices
            std::cout << "Vertices:\n";

            // Go through each vertex and print its number,
            //  position, normal, and texture coordinate
            for (unsigned int j = 0; j < curMesh.Vertices.size(); j++) {
                std::cout << "V" << j << ": " <<
                    "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                    "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                    "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";

                Vec3f v(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);
                m_verts.push_back(v);
                Vec3f n(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z);
                m_norms.push_back(n);
                Vec2f uv(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y);
                m_uv.push_back(uv);

            }

            // Print Indices
            std::cout << "Indices:\n";

            // Go through every 3rd index and print the
            //    triangle that these indices represent
            for (unsigned int j = 0; j < curMesh.Indices.size(); j += 3) {
                std::cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";

                std::vector<Vec3i> f;
                for (unsigned int k = 0; k < 3; k++) {
                    Vec3i tmp(
                        curMesh.Indices[j + k], // Index of the vertex position
                        curMesh.Indices[j + k], // Index of the texture coordinate (uv)
                        curMesh.Indices[j + k]  // Index of the vertex normal
                    );
                    f.push_back(tmp);
                }
                m_faces.push_back(f);
            }

            // Print Material
            std::cout << "Material: " << curMesh.MeshMaterial.name << "\n";
            std::cout << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
            std::cout << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
            std::cout << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
            std::cout << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
            std::cout << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
            std::cout << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
            std::cout << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
            std::cout << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
            std::cout << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
            std::cout << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
            std::cout << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
            std::cout << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

            // Leave a space to separate from the next mesh
            std::cout << "\n";
        }

    return true;

}

Model::Model(const char *filename) {
    load_obj_model(filename);

    std::cerr << "# v# " << m_verts.size() << " f# "  << m_faces.size() << " vt# " << m_uv.size() << " vn# " << m_norms.size() << std::endl;
    load_texture(filename, ".jpg", m_diffusemap);
    //~ load_texture(filename, "_nm_tangent.png", m_normalmap);
    //~ load_texture(filename, "_spec.png", m_specularmap);
}


Model::~Model() {}

int Model::nverts() {
    return (int)m_verts.size();
}

int Model::nfaces() {
    return (int)m_faces.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)m_faces[idx].size(); i++) face.push_back(m_faces[idx][i][0]);
    return face;
}

Vec3f Model::vert(int i) {
    return m_verts[i];
}

Vec3f Model::vert(int iface, int nthvert) {
    return m_verts[m_faces[iface][nthvert][0]];
}

void Model::load_texture(std::string filename, const char *suffix, Image &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_from_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

ImageColor Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0]*m_diffusemap.get_width(), uvf[1]*m_diffusemap.get_height());
    return m_diffusemap.get(uv[0], uv[1]);
}

Vec3f Model::normal(Vec2f uvf) {
    if (m_normalmap.isEmpty()) {
        Vec3f res;
        res[0] = 1.0f;
        res[1] = 1.0f;
        res[2] = 1.0f;
        return res;
    }

    Vec2i uv(uvf[0]*m_normalmap.get_width(), uvf[1]*m_normalmap.get_height());
    ImageColor c = m_normalmap.get(uv[0], uv[1]);
    Vec3f res;
    for (int i=0; i<3; i++)
        res[2-i] = (float)c[i]/255.f*2.f - 1.f;
    return res;
}

Vec2f Model::uv(int iface, int nthvert) {
    return m_uv[m_faces[iface][nthvert][1]];
}

//~ float Model::specular(Vec2f uvf) {
//~     Vec2i uv(uvf[0]*m_specularmap.get_width(), uvf[1]*m_specularmap.get_height());
//~     return m_specularmap.get(uv[0], uv[1])[0]/1.f;
//~ }

Vec3f Model::normal(int iface, int nthvert) {
    int idx = m_faces[iface][nthvert][2];
    return m_norms[idx].normalize();
}

