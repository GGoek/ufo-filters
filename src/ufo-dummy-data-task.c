/*
 * Copyright (C) 2011-2013 Karlsruhe Institute of Technology
 *
 * This file is part of Ufo.
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "ufo-dummy-data-task.h"


struct _UfoDummyDataTaskPrivate {
    guint width;
    guint height;
    guint depth;
    guint number;
    guint bitdepth;
    guint current;
};

static void ufo_task_interface_init (UfoTaskIface *iface);

G_DEFINE_TYPE_WITH_CODE (UfoDummyDataTask, ufo_dummy_data_task, UFO_TYPE_TASK_NODE,
                         G_IMPLEMENT_INTERFACE (UFO_TYPE_TASK,
                                                ufo_task_interface_init))

#define UFO_DUMMY_DATA_TASK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_DUMMY_DATA_TASK, UfoDummyDataTaskPrivate))

enum {
    PROP_0,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_DEPTH,
    PROP_NUMBER,
    PROP_BITDEPTH,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

UfoNode *
ufo_dummy_data_task_new (void)
{
    return UFO_NODE (g_object_new (UFO_TYPE_DUMMY_DATA_TASK, NULL));
}

static void
ufo_dummy_data_task_setup (UfoTask *task,
                         UfoResources *resources,
                         GError **error)
{
    UfoDummyDataTaskPrivate *priv;

    priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE (task);
    priv->current = 0;
}

static void
ufo_dummy_data_task_get_requisition (UfoTask *task,
                                   UfoBuffer **inputs,
                                   UfoRequisition *requisition)
{
    UfoDummyDataTaskPrivate *priv;

    priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE (task);

    requisition->n_dims = 2;
    requisition->dims[0] = priv->width;
    requisition->dims[1] = priv->height;

    if (priv->depth > 2) {
        requisition->n_dims += 1;
        requisition->dims[2] = priv->depth;
    }
}

static guint
ufo_dummy_data_task_get_num_inputs (UfoTask *task)
{
    return 0;
}

static guint
ufo_dummy_data_task_get_num_dimensions (UfoTask *task,
                               guint input)
{
    return 0;
}

static UfoTaskMode
ufo_dummy_data_task_get_mode (UfoTask *task)
{
    return UFO_TASK_MODE_GENERATOR | UFO_TASK_MODE_CPU;
}

static gboolean
ufo_dummy_data_task_generate (UfoTask *task,
                            UfoBuffer *output,
                            UfoRequisition *requisition)
{
    UfoDummyDataTaskPrivate *priv;

    priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE (task);

    if (priv->current == priv->number)
        return FALSE;

    if (priv->bitdepth == 8 || priv->bitdepth == 16) {
        gfloat *array;

        /* force getting the data, otherwise no conversion will take place */
        array = ufo_buffer_get_host_array (output, NULL);
        array[0] = 0.0;

        ufo_buffer_convert (output, priv->bitdepth == 8 ? UFO_BUFFER_DEPTH_8U : UFO_BUFFER_DEPTH_16U);
    }

    priv->current++;

    return TRUE;
}

static void
ufo_dummy_data_task_set_property (GObject *object,
                                guint property_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
    UfoDummyDataTaskPrivate *priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_WIDTH:
            priv->width = g_value_get_uint (value);
            break;
        case PROP_HEIGHT:
            priv->height = g_value_get_uint (value);
            break;
        case PROP_DEPTH:
            priv->depth = g_value_get_uint (value);
            break;
        case PROP_NUMBER:
            priv->number = g_value_get_uint (value);
            break;
        case PROP_BITDEPTH:
            {
                guint depth;

                depth = g_value_get_uint (value);

                if (depth != 8 && depth != 16 && depth != 32)
                    g_warning ("::bitdepth must be either 8, 16 or 32");
                else
                    priv->bitdepth = depth;
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_dummy_data_task_get_property (GObject *object,
                                guint property_id,
                                GValue *value,
                                GParamSpec *pspec)
{
    UfoDummyDataTaskPrivate *priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_WIDTH:
            g_value_set_uint (value, priv->width);
            break;
        case PROP_HEIGHT:
            g_value_set_uint (value, priv->height);
            break;
        case PROP_DEPTH:
            g_value_set_uint (value, priv->depth);
            break;
        case PROP_NUMBER:
            g_value_set_uint (value, priv->number);
            break;
        case PROP_BITDEPTH:
            g_value_set_uint (value, priv->bitdepth);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_dummy_data_task_finalize (GObject *object)
{
    G_OBJECT_CLASS (ufo_dummy_data_task_parent_class)->finalize (object);
}

static void
ufo_task_interface_init (UfoTaskIface *iface)
{
    iface->setup = ufo_dummy_data_task_setup;
    iface->get_num_inputs = ufo_dummy_data_task_get_num_inputs;
    iface->get_num_dimensions = ufo_dummy_data_task_get_num_dimensions;
    iface->get_mode = ufo_dummy_data_task_get_mode;
    iface->get_requisition = ufo_dummy_data_task_get_requisition;
    iface->generate = ufo_dummy_data_task_generate;
}

static void
ufo_dummy_data_task_class_init (UfoDummyDataTaskClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = ufo_dummy_data_task_set_property;
    gobject_class->get_property = ufo_dummy_data_task_get_property;
    gobject_class->finalize = ufo_dummy_data_task_finalize;

    properties[PROP_WIDTH] =
        g_param_spec_uint ("width",
                           "Width of the buffer",
                           "Width of the buffer",
                           1, 2 << 16, 1,
                           G_PARAM_READWRITE);

    properties[PROP_HEIGHT] =
        g_param_spec_uint ("height",
                           "Height of the buffer",
                           "Height of the buffer",
                           1, 2 << 16, 1,
                           G_PARAM_READWRITE);

    properties[PROP_DEPTH] =
        g_param_spec_uint ("depth",
                           "Depth of the buffer",
                           "Depth of the buffer",
                           1, 2 << 16, 1,
                           G_PARAM_READWRITE);

    properties[PROP_NUMBER] =
        g_param_spec_uint ("number",
                           "Number of buffers",
                           "Number of buffers",
                           1, 2 << 16, 1,
                           G_PARAM_READWRITE);

    properties[PROP_BITDEPTH] =
        g_param_spec_uint ("bitdepth",
                           "Number of bits",
                           "Number of bits, to simulate the effect of implicit conversion",
                           8, 32, 32,
                           G_PARAM_READWRITE);

    for (guint i = PROP_0 + 1; i < N_PROPERTIES; i++)
        g_object_class_install_property (gobject_class, i, properties[i]);

    g_type_class_add_private (gobject_class, sizeof(UfoDummyDataTaskPrivate));
}

static void
ufo_dummy_data_task_init(UfoDummyDataTask *self)
{
    self->priv = UFO_DUMMY_DATA_TASK_GET_PRIVATE(self);
    self->priv->width = 1;
    self->priv->height = 1;
    self->priv->depth = 1;
    self->priv->number = 1;
    self->priv->current = 0;
    self->priv->bitdepth = 32;
}
