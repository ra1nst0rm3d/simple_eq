/**
 * SECTION:element-simpleeq
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch audiotestsrc ! audioconverter ! simpleeq ! ! audioconverter ! autoaudiosink
 * ]|
 * </refsect2>
 */

#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/base/basetransform.h>
#include <gst/audio/audio.h>
#include <gst/audio/gstaudiofilter.h>
#include <pthread.h>
#include "gst_simpleeq.h"


GST_DEBUG_CATEGORY_STATIC (gst_simpleeq_debug);
#define GST_CAT_DEFAULT gst_simpleeq_debug;

enum {
    PROP_COEFF,
    PROP_SMPL
};

#define gst_simpleeq_parent_class parent_class
G_DEFINE_TYPE (GstSimpleEQ, gst_simpleeq, GST_TYPE_AUDIO_FILTER);

tatic void gst_simpleeq_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec);

static void gst_simpleeq_get_property(GObject *object, guint prop_id,
                                     GValue *value, GParamSpec *pspec);

static void gst_simpleeq_finalize(GObject *object);

static gboolean gst_simpleeq_setup(GstAudioFilter *self,
                                  const GstAudioInfo *info);

static gboolean gst_simpleeq_stop(GstBaseTransform *base);

static GstFlowReturn gst_simpleeq_preprocess(GstBaseTransform *base,
                                              GstBuffer *outbuf);


static void
gst_simpleeq_class_init (GstSimpleEQClass *klass) {
    GObjectClass *gobj = (GObjectClass *) klass;
    GstElementClass *gstelement_class = (GstElementClass *) klass;
    GstBaseTransformClass *basetransform_class = (GstBaseTransformClass *) klass;
    GstAudioFilterClass *audioself_class = (GstAudioFilterClass *) klass;
    GstCaps *caps;

    GST_DEBUG_CATEGORY_INIT (gst_simpleeq_debug, "SimpleEQ", 0, "SimpleEQ element");

    gobj->set_property = gst_simpleeq_set_property;
    gobj->get_property = gst_simpleeq_get_property;
    gobj->finalize = gst_simpleeq_finalize;

    g_object_class_install_property(gobj, PROP_COEFF, 
                                    g_param_spec_value_array("coeff", "Coefficients", "Filter coefficients", g_param_spec_double ("Coeff", "Filter coefficient", "Filter coefficient", -G_MAXDOUBLE, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS), G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(gobj, PROP_SMPL, g_param_spec_int("samplerate", "SampleRate", "Sampling Rate", 0, 500000, 0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_CONTROLLABLE));
    
    caps = gst_caps_from_string(ALLOWED_CAPS);
    gst_audio_filter_class_add_pad_templates(GST_SIMPLEEQ_CLASS (klass), caps);
    gst_caps_unref(caps);

    audioself_class->setup = GST_DEBUG_FUNCPTR (gst_simpleeq_setup);
    basetransform_class->transform_ip = GST_DEBUG_FUNCPTR (gst_simpleeq_preprocess);
    basetransform_class->transform_ip_on_passthrough = FALSE;
    basetransform_class->stop = GST_DEBUG_FUNCPTR (gst_simpleeq_stop);
}

static void
gst_simpleeq_init (GstSimpleEQ *self) {
    gint32 idx;

    gst_base_transform_set_in_place(GST_BASE_TRANSFORM (self), TRUE);
    gst_base_transform_set_gap_aware(GST_BASE_TRANSFORM (self), TRUE);

    self->coeffs = g_value_array_new (6);
    self->samplerate = 0;

    g_mutex_init(&self->lock);
}

static void
gst_simpleeq_finalize (GObject *object) {
    GstSimpleEQ *self = GST_SIMPLEEQ (object);

    if(self->coeffs != NULL) {
        g_value_array_free(self->coeffs);
        self->coeffs = NULL;
    }

    g_mutex_clear(&self->lock);
    G_OBJECT_CLASS (parent_class)->finalize(object);
}

static void
gst_simpleeq_set_property(GObject *object, guint prop_id,
                         const GValue *value, GParamSpec *pspec) {
    GstSimpleEQ *self = GST_SIMPLEEQ (object);
    switch(prop_id) {
        case PROP_COEFF: {
            g_mutex_lock(&self->lock);
            self->coeffs = g_value_dup_boxed(value);
            g_mutex_unlock(&self->lock);
        }
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gst_simpleeq_get_property(GObject *object, guint prop_id,
                         GValue *value, GParamSpec *pspec) {
    GstSimpleEQ *self = GST_SIMPLEEQ(object);

    switch (prop_id) {
        case PROP_COEFF:
            g_value_set_boxed(value, self->coeffs);
            break;
        case PROP_SMPL:
            g_value_set_int(value, self->samplerate);
            break;
    }
}

static gboolean
gst_simpleeq_setup (GstAudioFilter *base, const GstAudioInfo *info) {
    GstSimpleEQ *self = GST_SIMPLEEQ (base);


    if(info) {
        self->samplerate = GST_AUDIO_INFO_RATE (info);
    } else {
        self->samplerate = GST_AUDIO_FILTER_RATE (self);
    }
    if(self->samplerate <= 0) {
        return FALSE;
    }

    GST_DEBUG_OBJECT (self, "current sample_rate = %d", self->samplerate);

    return TRUE;
}

static gboolean
gst_viperfx_stop (GstBaseTransform *base) {
    GstSimpleEQ *self = GST_SIMPLEEQ (base);

    return TRUE;
}

static GstFlowReturn
gst_simpleeq_preprocess(GstBaseTransform *base, GstBuffer *buf) {
    GstSimpleEQ *filt = GST_SIMPLEEQ (base);
    guint idx,num_samples;
    double *pcm_data;
    GstClockTime timestamp,stream_time;
    GstMapInfo map;
    double bin0,bin1,bout0,bout1;
    timestamp = GST_BUFFER_TIMESTAMP (buf);
    stream_time = gst_segment_to_stream_time(&base->segment, GST_FORMAT_TIME, timestamp);

    if (GST_CLOCK_TIME_IS_VALID (stream_time))
        gst_object_sync_values(GST_OBJECT (filter), stream_time);

    if (G_UNLIKELY (GST_BUFFER_FLAG_IS_SET(buf, GST_BUFFER_FLAG_GAP)))
        return GST_FLOW_OK;

    gst_buffer_map(buf, &map, GST_MAP_READWRITE);
    num_samples = map.size / GST_AUDIO_FILTER_BPS (filter) / 2;
    pcm_data = (double*) map.data;
    for(int idx = 0; idx < num_samples / 2; idx++) {
        *(buf + idx) = (b0/a0) * *(pcm_data + idx) + (b1/a0) * bin0 + (b2/a0) * bin1 - (a1/a0) * bout0 - (a2/a0) * bout1;
        bin1 = bin0;
        bin0 = *(pcm_data + idx);
        bout1 = bout0;
        bout0 = *(buf + idx); 
    }
    bout1 = bout0 = bin0 = bin1 = 0;
    for(idx; idx < num_samples; idx++) {
        *(buf + idx) = (b0/a0) * *(pcm_data + idx) + (b1/a0) * bin0 + (b2/a0) * bin1 - (a1/a0) * bout0 - (a2/a0) * bout1;
        bin1 = bin0;
        bin0 = *(pcm_data + idx);
        bout1 = bout0;
        bout0 = *(buf + idx);
    }

    gst_buffer_unmap(buf, &map);
    return GST_FLOW_OK;
}

static gboolean
viperfx_init (GstPlugin *SimpleEQ) {
    return gst_element_register(SimpleEQ, "SimpleEQ", GST_RANK_NONE, GST_TYPE_SIMPLEEQ);
}

GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  simpleeq,
  "SimpleEQ",
  simpleeq_init,
  VERSION,
  "Proprietary",
  "GStreamer",
  "http://gstreamer.net/"
)
