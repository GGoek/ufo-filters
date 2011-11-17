#ifndef __UFO_FILTER_COPY_H
#define __UFO_FILTER_COPY_H

#include <glib.h>
#include <glib-object.h>

#include <ufo/ufo-filter.h>

#define UFO_TYPE_FILTER_COPY             (ufo_filter_copy_get_type())
#define UFO_FILTER_COPY(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), UFO_TYPE_FILTER_COPY, UfoFilterCopy))
#define UFO_IS_FILTER_COPY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), UFO_TYPE_FILTER_COPY))
#define UFO_FILTER_COPY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), UFO_TYPE_FILTER_COPY, UfoFilterCopyClass))
#define UFO_IS_FILTER_COPY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), UFO_TYPE_FILTER_COPY))
#define UFO_FILTER_COPY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), UFO_TYPE_FILTER_COPY, UfoFilterCopyClass))

typedef struct _UfoFilterCopy           UfoFilterCopy;
typedef struct _UfoFilterCopyClass      UfoFilterCopyClass;
typedef struct _UfoFilterCopyPrivate    UfoFilterCopyPrivate;

struct _UfoFilterCopy {
    UfoFilter parent_instance;

    /* private */
    UfoFilterCopyPrivate *priv;
};

struct _UfoFilterCopyClass {
    UfoFilterClass parent_class;
};

/* virtual public methods */

/* non-virtual public methods */

GType ufo_filter_copy_get_type(void);

#endif