#include <gmodule.h>
#include <string.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <ufo/ufo-resource-manager.h>
#include <ufo/ufo-filter.h>
#include <ufo/ufo-buffer.h>
#include "ufo-filter-region-of-interest.h"

/**
 * SECTION:ufo-filter-region-of-interest
 * @Short_description: Cut out a region of interest
 * @Title: regionofinterest
 *
 * Cut out a region of interest from any two-dimensional input. If the ROI is
 * (partially) outside the input, only data accessible will be copied.
 */

struct _UfoFilterRegionOfInterestPrivate {
    guint x;
    guint y;
    guint width;
    guint height;
};

G_DEFINE_TYPE(UfoFilterRegionOfInterest, ufo_filter_region_of_interest, UFO_TYPE_FILTER)

#define UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_FILTER_REGION_OF_INTEREST, UfoFilterRegionOfInterestPrivate))

enum {
    PROP_0,
    PROP_X,
    PROP_Y,
    PROP_WIDTH,
    PROP_HEIGHT,
    N_PROPERTIES
};

static GParamSpec *region_of_interest_properties[N_PROPERTIES] = { NULL, };

static GError *ufo_filter_region_of_interest_initialize(UfoFilter *filter, UfoBuffer *inputs[], guint **dims)
{
    UfoFilterRegionOfInterestPrivate *priv = UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(filter);
    dims[0][0] = priv->width;
    dims[0][1] = priv->height;
    return NULL;
}

static GError *ufo_filter_region_of_interest_process_cpu(UfoFilter *filter,
        UfoBuffer *inputs[], UfoBuffer *outputs[], gpointer cmd_queue)
{
    UfoFilterRegionOfInterestPrivate *priv = UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(filter);
    guint x1 = priv->x, y1 = priv->y;
    guint x2 = x1 + priv->width, y2 = y1 + priv->height;
    guint in_width, in_height;

    ufo_buffer_get_2d_dimensions(inputs[0], &in_width, &in_height);

    /* Don't do anything if we are completely out of bounds */
    if (x1 > in_width || y1 > in_height) {
        /* XXX: Maybe issue an error? */
        return NULL;
    }

    guint rd_width = x2 > in_width ? in_width - x1 : priv->width;
    guint rd_height = y2 > in_height ? in_height - y1 : priv->height;
    gfloat *in_data = ufo_buffer_get_host_array(inputs[0], (cl_command_queue) cmd_queue);
    gfloat *out_data = ufo_buffer_get_host_array(outputs[0], (cl_command_queue) cmd_queue);

    /*
     * Removing the for loop for "width aligned" regions gives a marginal
     * speed-up of ~4 per cent.
     */
    if (rd_width == in_width) {
        g_memmove(out_data, in_data + y1*in_width, 
                rd_width * rd_height * sizeof(gfloat));
    }
    else {
        for (guint y = 0; y < rd_height; y++) {
            g_memmove(out_data + y*priv->width, in_data + (y + y1)*in_width + x1, 
                    rd_width * sizeof(gfloat));
        }
    }

    return NULL;
}

static void ufo_filter_region_of_interest_set_property(GObject *object,
    guint           property_id,
    const GValue    *value,
    GParamSpec      *pspec)
{
    UfoFilterRegionOfInterestPrivate *priv = UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(object);

    switch (property_id) {
        case PROP_X:
            priv->x = g_value_get_uint(value);
            break;
        case PROP_Y:
            priv->y = g_value_get_uint(value);
            break;
        case PROP_WIDTH:
            priv->width = g_value_get_uint(value);
            break;
        case PROP_HEIGHT:
            priv->height = g_value_get_uint(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void ufo_filter_region_of_interest_get_property(GObject *object,
    guint       property_id,
    GValue      *value,
    GParamSpec  *pspec)
{
    UfoFilterRegionOfInterestPrivate *priv = UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(object);

    switch (property_id) {
        case PROP_X:
            g_value_set_uint(value, priv->x);
            break;
        case PROP_Y:
            g_value_set_uint(value, priv->y);
            break;
        case PROP_WIDTH:
            g_value_set_uint(value, priv->width);
            break;
        case PROP_HEIGHT:
            g_value_set_uint(value, priv->height);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void ufo_filter_region_of_interest_class_init(UfoFilterRegionOfInterestClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    UfoFilterClass *filter_class = UFO_FILTER_CLASS(klass);

    gobject_class->set_property = ufo_filter_region_of_interest_set_property;
    gobject_class->get_property = ufo_filter_region_of_interest_get_property;
    filter_class->initialize = ufo_filter_region_of_interest_initialize;
    filter_class->process_cpu = ufo_filter_region_of_interest_process_cpu;

    region_of_interest_properties[PROP_X] = 
        g_param_spec_uint("x",
            "Horizontal coordinate",
            "Horizontal coordinate from where to read input",
            0, G_MAXUINT, 0,
            G_PARAM_READWRITE);

    region_of_interest_properties[PROP_Y] = 
        g_param_spec_uint("y",
            "Vertical coordinate",
            "Vertical coordinate from where to read input",
            0, G_MAXUINT, 0,
            G_PARAM_READWRITE);

    region_of_interest_properties[PROP_WIDTH] = 
        g_param_spec_uint("width",
            "Width",
            "Width of the region of interest",
            1, G_MAXUINT, 256,
            G_PARAM_READWRITE);

    region_of_interest_properties[PROP_HEIGHT] = 
        g_param_spec_uint("height",
            "Height",
            "Height of the region of interest",
            1, G_MAXUINT, 256,
            G_PARAM_READWRITE);

    for (guint i = PROP_0 + 1; i < N_PROPERTIES; i++)
        g_object_class_install_property(gobject_class, i, region_of_interest_properties[i]);

    g_type_class_add_private(gobject_class, sizeof(UfoFilterRegionOfInterestPrivate));
}

static void ufo_filter_region_of_interest_init(UfoFilterRegionOfInterest *self)
{
    UfoFilterRegionOfInterestPrivate *priv = self->priv = UFO_FILTER_REGION_OF_INTEREST_GET_PRIVATE(self);
    priv->x = 0;
    priv->y = 0;
    priv->width = 256;
    priv->height = 256;

    ufo_filter_register_inputs(UFO_FILTER(self), 2, NULL);
    ufo_filter_register_outputs(UFO_FILTER(self), 2, NULL);
}

G_MODULE_EXPORT UfoFilter *ufo_filter_plugin_new(void)
{
    return g_object_new(UFO_TYPE_FILTER_REGION_OF_INTEREST, NULL);
}
