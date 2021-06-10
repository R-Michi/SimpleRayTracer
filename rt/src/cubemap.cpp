#include "../cubemap.h"

using namespace rt;

Cubemap::Cubemap(void)
{
    this->filter = RT_FILTER_LINEAR;
}

Cubemap::Cubemap(const CubemapCreateInfo& info)
{
    this->load(info);
}

Cubemap::~Cubemap(void)
{
    this->clear();
}

TextureError Cubemap::load_face(CubemapFace face, const char* path, uint32_t channels)
{
    int w, h, c;
    Image image;

    image.data = stbi_load(path, &w, &h, &c, channels);         // force load a certain numbero of channels
    if (image.data == nullptr) return RT_TEXTURE_ERROR_LOAD;
    image.layout.width = static_cast<uint32_t>(w);
    image.layout.height = static_cast<uint32_t>(h);
    image.layout.channels = channels;
    this->faces[face] = image;
    return RT_TEXTURE_ERROR_NONE;
}

TextureError Cubemap::load(const CubemapCreateInfo& info)
{
    this->clear();
    TextureError error;

    // forceload 4 channels (RGBA)
    if((error = this->load_face(RT_CUBEMAP_FACE_FRONT, info.front, 4))      != RT_TEXTURE_ERROR_NONE) return error;
    if((error = this->load_face(RT_CUBEMAP_FACE_BACK, info.back, 4))        != RT_TEXTURE_ERROR_NONE) return error;
    if((error = this->load_face(RT_CUBEMAP_FACE_TOP, info.top, 4))          != RT_TEXTURE_ERROR_NONE) return error;
    if((error = this->load_face(RT_CUBEMAP_FACE_BOTTOM, info.bottom, 4))    != RT_TEXTURE_ERROR_NONE) return error;
    if((error = this->load_face(RT_CUBEMAP_FACE_LEFT, info.left, 4))        != RT_TEXTURE_ERROR_NONE) return error;
    if((error = this->load_face(RT_CUBEMAP_FACE_RIGHT, info.right, 4))      != RT_TEXTURE_ERROR_NONE) return error;
    this->filter = info.filter;

    return RT_TEXTURE_ERROR_NONE;
}

void Cubemap::clear(void)
{
    // free all images
    for (auto iter = this->faces.begin(); iter != this->faces.end(); iter++)
    {
        stbi_image_free(iter->second.data);
    }
    this->faces.clear();
}

glm::vec2 Cubemap::get_cube_uv(const glm::vec3& dir, CubemapFace& face)
{
    const glm::vec3 abs_dir = glm::abs(dir);
    bool is_x_positive = (dir.x >= 0.0f);
    bool is_y_positive = (dir.y >= 0.0f);
    bool is_z_positive = (dir.z >= 0.0f);

    float max_axis;
    glm::vec2 uv;

    // front face / positive X
    if (is_x_positive && abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z)
    {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        max_axis = abs_dir.x;
        uv.x = -dir.z;
        uv.y = dir.y;
        face = RT_CUBEMAP_FACE_FRONT;
    }

    // back face / negative X
    if (!is_x_positive && abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z)
    {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        max_axis = abs_dir.x;
        uv.x = dir.z;
        uv.y = dir.y;
        face = RT_CUBEMAP_FACE_BACK;
    }

    // top face / postive Y
    if (is_y_positive && abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        max_axis = abs_dir.y;
        uv.x = dir.x;
        uv.y = -dir.z;
        face = RT_CUBEMAP_FACE_TOP;
    }

    // bottom face / negative Y
    if (!is_y_positive && abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        max_axis = abs_dir.y;
        uv.x = dir.x;
        uv.y = dir.z;
        face = RT_CUBEMAP_FACE_BOTTOM;
    }

    // left face / postive Z
    if (is_z_positive && abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        max_axis = abs_dir.z;
        uv.x = dir.x;
        uv.y = dir.y;
        face = RT_CUBEMAP_FACE_LEFT;
    }

    // right face / negative Z
    if (!is_z_positive && abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y)
    {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        max_axis = abs_dir.z;
        uv.x = -dir.x;
        uv.y = dir.y;
        face = RT_CUBEMAP_FACE_RIGHT;
    }

    // uv coordinates have a range of -1 to +1 -> convert to 0 to +1
    uv = 0.5f * (uv / max_axis + 1.0f);
    uv.y = 1.0f - uv.y;
    return uv;
}

void Cubemap::get_color(const Image& image, const glm::uvec2& px_coord, glm::vec4& color)
{
    if (px_coord.x >= image.layout.width || px_coord.y >= image.layout.height)
    {
        color = { 0.0f, 0.0f, 0.0f, 0.0f };
        return;
    }

    const uint32_t image_stride = image.layout.width * image.layout.channels;
    const uint32_t image_pos_x = px_coord.x * image.layout.channels;
    const uint32_t image_pos_y = px_coord.y;

    const uint8_t r = image.data[image_pos_y * image_stride + image_pos_x + 0];
    const uint8_t g = image.data[image_pos_y * image_stride + image_pos_x + 1];
    const uint8_t b = image.data[image_pos_y * image_stride + image_pos_x + 2];
    const uint8_t a = image.data[image_pos_y * image_stride + image_pos_x + 3];

    static constexpr float incree = 1.0f / 255.0f;
    color = { r * incree, g * incree, b * incree, a * incree };
}

glm::vec4 Cubemap::sample(const glm::vec3& direction)
{
    CubemapFace face;
    glm::vec2 uv = this->get_cube_uv(direction, face);
    const Image& image = this->faces[face];

    // convert uv coordinates to pixel coordinates, range: 0 to width-1 or height-1
    glm::vec2 image_size(static_cast<float>(image.layout.width - 1), static_cast<float>(image.layout.height - 1));
    glm::vec2 px_coords = uv * image_size;

    // integral part -> used to sample the pixel
    glm::uvec2 px_int = glm::floor(px_coords);

    glm::vec4 color;
    if (this->filter == RT_FILTER_NEAREST)
    {
        this->get_color(image, px_int, color);
    }
    else if (this->filter == RT_FILTER_LINEAR)
    {
        // fractional part of pixel coordinates -> used for linear filtering
        glm::vec2 px_fract = glm::fract(px_coords);

        // get pixel colors
        glm::vec4 c1, c2, c3, c4;
        this->get_color(image, px_int, c1);
        this->get_color(image, glm::vec2(px_int.x + 1, px_int.y), c2);
        this->get_color(image, glm::vec2(px_int.x, px_int.y + 1), c3);
        this->get_color(image, glm::vec2(px_int.x + 1, px_int.y + 1), c4);

        // interpolate colors
        glm::vec4 c12, c34;
        c12 = glm::mix(c1, c2, px_fract.x);
        c34 = glm::mix(c3, c4, px_fract.x);
        color = glm::mix(c12, c34, px_fract.y);
    }

    //std::cout << "R:" << color.r << "  G:" << color.g << "  B:" << color.b << std::endl;
    return color;
}