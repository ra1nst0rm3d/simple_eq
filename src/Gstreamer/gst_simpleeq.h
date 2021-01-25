#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#ifndef __GST_SIMPLEEQ_H__
#define __GST_SIMPLEEQ_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/audio/audio.h>
#include <gst/audio/gstaudiofilter.h>
//#include "SIMPLEEQ_so.h"


G_BEGIN_DECLS

#define GST_TYPE_SIMPLEEQ            (gst_simpleeq_get_type())
#define GST_SIMPLEEQ(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_SIMPLEEQ,GstSimpleEQ))
#define GST_SIMPLEEQ_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_SIMPLEEQ,GstSimpleEQClass))
#define GST_SIMPLEEQ_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_SIMPLEEQ,GstSimpleEQClass))
#define GST_IS_SIMPLEEQ(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_SIMPLEEQ))
#define GST_IS_SIMPLEEQ_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_SIMPLEEQ))

#define s filt->coeffs->values
#define b0 g_value_get_double(s + 0)
#define b1 g_value_get_double(s + 1)
#define b2 g_value_get_double(s + 2)
#define a0 g_value_get_double(s + 3)
#define a1 g_value_get_double(s + 4)
#define a2 g_value_get_double(s + 5)

#define PACKAGE "SimpleEQ"
#define VERSION "0.0.1"

#define ALLOWED_CAPS \
  "audio/x-raw,"                            \
  " format=(string){"GST_AUDIO_NE(S16)"},"  \
  " rate=(int)[44100,MAX],"                 \
  " channels=(int)2,"                       \
  " layout=(string)interleaved"



typedef struct _GstSimpleEQ      GstSimpleEQ;
typedef struct _GstSimpleEQClass GstSimpleEQClass;

struct _GstSimpleEQ {
    GstAudioFilter audiofilter;

    GValueArray *coeffs;

    /* < private > */
    GMutex lock;
    guint32 samplerate;
};

struct _GstSimpleEQClass {
    GstAudioFilterClass parent_class;
};

GType gst_simpleeq_get_type (void);

G_END_DECLS

#endif /* __GST_SIMPLEEQ_H__ */

#pragma clang diagnostic pop
