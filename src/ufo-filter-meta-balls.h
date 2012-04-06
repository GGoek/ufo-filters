#ifndef __UFO_FILTER_META_BALLS_H
#define __UFO_FILTER_META_BALLS_H

#include <glib.h>

#include <ufo/ufo-filter.h>

#define UFO_TYPE_FILTER_META_BALLS             (ufo_filter_meta_balls_get_type())
#define UFO_FILTER_META_BALLS(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), UFO_TYPE_FILTER_META_BALLS, UfoFilterMetaBalls))
#define UFO_IS_FILTER_META_BALLS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), UFO_TYPE_FILTER_META_BALLS))
#define UFO_FILTER_META_BALLS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), UFO_TYPE_FILTER_META_BALLS, UfoFilterMetaBallsClass))
#define UFO_IS_FILTER_META_BALLS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), UFO_TYPE_FILTER_META_BALLS))
#define UFO_FILTER_META_BALLS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), UFO_TYPE_FILTER_META_BALLS, UfoFilterMetaBallsClass))

typedef struct _UfoFilterMetaBalls           UfoFilterMetaBalls;
typedef struct _UfoFilterMetaBallsClass      UfoFilterMetaBallsClass;
typedef struct _UfoFilterMetaBallsPrivate    UfoFilterMetaBallsPrivate;

struct _UfoFilterMetaBalls {
    /*< private >*/
    UfoFilter parent_instance;

    UfoFilterMetaBallsPrivate *priv;
};

/**
 * UfoFilterMetaBallsClass:
 *
 * #UfoFilterMetaBalls class
 */
struct _UfoFilterMetaBallsClass {
    /*< private >*/
    UfoFilterClass parent_class;
};

GType ufo_filter_meta_balls_get_type(void);
UfoFilter *ufo_filter_plugin_new(void);

#endif