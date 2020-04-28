#include <vector>
#include <limits>
#include <iostream>

#include "inipp.h"
#include "image.h"
#include "model.h"
#include "geometry.h"
#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

double str2dbl(const char *s);

#ifdef __cplusplus
}
#endif

Model *model = NULL;

static int width  = 350;
static int height = 600;

static Vec3f light_dir(1,3,2);
static Vec3f       eye(3,3,3);
static Vec3f    center(0,0,0);
static Vec3f        up(0,1,0);

static double viewport_aperture = height * 0.816;
static double viewport_aspect = 1;

// Rendering

struct Shader : public IShader {
    mat<2,3,float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4,3,float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
    mat<3,3,float> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3,3,float> ndc_tri;     // triangle in normalized device coordinates

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((Projection*ModelView).invert_transpose()*embed<4>(model->normal(iface, nthvert), 0.f)));
        Vec4f gl_Vertex = Projection*ModelView*embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex/gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, ImageColor &color) {
        Vec3f bn = (varying_nrm*bar).normalize();
        Vec2f uv = varying_uv*bar;

        mat<3,3,float> A;
        A[0] = ndc_tri.col(1) - ndc_tri.col(0);
        A[1] = ndc_tri.col(2) - ndc_tri.col(0);
        A[2] = bn;

        mat<3,3,float> AI = A.invert();

        Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

        mat<3,3,float> B;
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, bn);

        Vec3f n = (B*model->normal(uv)).normalize();

        float diff = std::max(0.f, n*light_dir);
        color = model->diffuse(uv)*diff;

        return false;
    }
};

// Configuration

static bool endsWith(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

static std::vector<std::string> split(const std::string & s, const std::string & delimiter, const bool & removeEmptyEntries = false) {
    std::vector<std::string> tokens;

    for (size_t start = 0, end; start < s.length(); start = end + delimiter.length()) {
         size_t position = s.find(delimiter, start);
         end = position != std::string::npos ? position : s.length();

         std::string token = s.substr(start, end - start);
         if (!removeEmptyEntries || !token.empty()) {
             tokens.push_back(token);
         }
    }

    if (!removeEmptyEntries && (s.empty() || endsWith(s, delimiter))) {
        tokens.push_back("");
    }

    return tokens;
}

static bool parseVec3f(const std::string str, Vec3f & v) {
    if (str.length()) {
        std::vector<std::string> subs = split(str, ",", false);
        int i = 0;
        for (auto & sub : subs) {
            v[i++] = str2dbl(sub.c_str());
        }
    }
    return true;
}

static void readConfig(const std::string filename) {
    inipp::Ini<char> ini;
    std::ifstream is(filename);
    ini.parse(is);
    std::cout << "config file:" << std::endl;
    ini.default_section(ini.sections["CONFIG"]);
    ini.interpolate();
    ini.generate(std::cout);

    inipp::extract(ini.sections["CONFIG"]["width"], width);
    inipp::extract(ini.sections["CONFIG"]["height"], height);
    //~ std::cout << width;
    //~ std::cout << height;

    inipp::extract(ini.sections["CONFIG"]["aperture"], viewport_aperture);
    inipp::extract(ini.sections["CONFIG"]["aspect"], viewport_aspect);

    std::string str;

    inipp::extract(ini.sections["CONFIG"]["light_dir"], str);
    parseVec3f(str, light_dir);
    std::cout << light_dir;

    inipp::extract(ini.sections["CONFIG"]["eye_pos"], str);
    parseVec3f(str, eye);
    std::cout << eye;

    inipp::extract(ini.sections["CONFIG"]["center_pos"], str);
    parseVec3f(str, center);
    std::cout << center;

    inipp::extract(ini.sections["CONFIG"]["up_dir"], str);
    parseVec3f(str, up);
    std::cout << up;
}

// Main program

int main (int argc, const char * const * argv, const char * const * envp) {
    readConfig("config.ini");

    float *zbuffer = new float[width*height];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    Image frame(width, height, Image::RGBA);
    lookat(eye, center, up);

    double viewport_aspect_sq = sqrt(fabs(viewport_aspect));
    viewport(-width * 0.2, height * 0.092, viewport_aperture * viewport_aspect_sq, viewport_aperture / viewport_aspect_sq);

    projection(-1.f/(eye-center).norm());
    light_dir = proj<3>((Projection*ModelView*embed<4>(light_dir, 0.f))).normalize();

    for (int m=1; m<argc; m++) {
        model = new Model(argv[m]);
        Shader shader;
        for (int i=0; i<model->nfaces(); i++) {
            for (int j=0; j<3; j++) {
                shader.vertex(i, j);
            }
            triangle(shader.varying_tri, shader, frame, zbuffer);
        }
        delete model;
    }
    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_to_file("framebuffer.png");

    delete [] zbuffer;
    return EXIT_SUCCESS;
}

