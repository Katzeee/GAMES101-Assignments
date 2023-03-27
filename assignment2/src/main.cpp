// clang-format off
#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // TODO: Copy-paste your implementation from the previous assignment.
    zNear = -zNear;
    zFar = -zFar;
    Eigen::Matrix4f p2o;
    Eigen::Matrix4f o_scale;
    Eigen::Matrix4f o2center;
    auto tan_res = std::tan(MY_PI * eye_fov / 180 / 2);
    o_scale << std::fabs(1 / zNear / tan_res / aspect_ratio), 0, 0, 0,
               0, std::fabs(1 / zNear / tan_res), 0, 0,
               0, 0, std::fabs(2 / zNear - zFar), 0,
               0, 0, 0, 1;
    o2center << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, -(zNear + zFar) / 2,
                0, 0, 0, 1;
    p2o << zNear, 0, 0, 0,
           0, zNear, 0, 0,
           0, 0, zNear + zFar, -zFar * zNear,
           0, 0, 1, 0;
    return o_scale * o2center * p2o;
    // float n = -zNear, f = -zFar;
    //   float half_of_theta = eye_fov * MY_PI / 180.0f / 2;
    //   float t = std::abs(n) * std::tan(half_of_theta), b = -t;
    //   float r = t * aspect_ratio, l = -r;
    //   Eigen::Matrix4f Mp2o, Mtranslate, Mscale;
    //   Mscale << 2 / (r - l), 0, 0, 0,
    //         0, 2 / (t - b), 0, 0,
    //         0, 0, 2 / (n - f), 0,
    //         0, 0, 0, 1;
    //
    //     Mtranslate << 1, 0, 0, -(r + l) / 2,
    //         0, 1, 0, -(t + b) / 2,
    //         0, 0, 1, -(n + f) / 2,
    //         0, 0, 0, 1;
    //
    //     Mp2o << n, 0, 0, 0,
    //         0, n, 0, 0,
    //         0, 0, n + f, -(n * f),
    //         0, 0, 1, 0;
    //
    //     return Mscale * Mtranslate * Mp2o;

}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc == 2)
    {
        command_line = true;
        filename = std::string(argv[1]);
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0,0,5};


    std::vector<Eigen::Vector3f> pos
            {
                    {2, 0, -2},
                    {0, 2, -2},
                    {-2, 0, -2},
                    {3.5, -1, -5},
                    {2.5, 1.5, -5},
                    {-1, 0.5, -5}
            };

    std::vector<Eigen::Vector3i> ind
            {
                    {0, 1, 2},
                    {3, 4, 5}
            };

    std::vector<Eigen::Vector3f> cols
            {
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0}
            };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
    }

    return 0;
}
// clang-format on
