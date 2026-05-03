#include <iostream>

#include "materials/Material.hpp"
#include "samplers/Sampler.hpp"
#include "image/Image.hpp"
#include "utilities/RGBColor.hpp"
#include "utilities/Ray.hpp"
#include "utilities/ShadeInfo.hpp"
#include "world/World.hpp"
#include "world/ViewPlane.hpp"
#include "tracers/Tracer.hpp"

int main(int argc, char **argv) {
    World world;
    world.build();

    Sampler*   sampler   = world.sampler_ptr;
    Tracer*    tracer    = world.tracer_ptr;
    ViewPlane& viewplane = world.vplane;
    Image image(viewplane);

    bool use_tracer = (tracer != nullptr);

    std::vector<Ray> rays;
    int total = viewplane.hres * viewplane.vres;
    int done  = 0;
    int last_pct = -1;

    for (int x = 0; x < viewplane.hres; x++) {
        for (int y = 0; y < viewplane.vres; y++) {

            RGBColor pixel_color(0);
            rays = sampler->get_rays(x, y);

            for (const auto& ray : rays) {
                float weight = ray.w;
                RGBColor c;

                if (use_tracer) {
                    c = tracer->trace_ray(ray, 0);
                } else {
                    ShadeInfo sinfo = world.hit_objects(ray);
                    c = sinfo.hit ? sinfo.material_ptr->shade(sinfo) : world.bg_color;
                }

                pixel_color += c * weight;
            }

            image.set_pixel(x, y, pixel_color);
        }

        done += viewplane.vres;
        int pct = (done * 100) / total;
        if (pct != last_pct) {
            std::cout << pct << "% complete\n" << std::flush;
            last_pct = pct;
        }
    }

    image.write_png("scene.png");
    std::cout << "Done. Wrote scene.png\n";
    return 0;
}