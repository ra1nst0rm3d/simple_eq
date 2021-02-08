#include <libconfig.h++>
#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/gstcaps.h>
#include "Filter.hh"
#include <iostream>
#define DEFAULT_NAME "eq.cfg"
using namespace std;
using namespace libconfig;

static double* data;
void process() {
    size_t size = sizeof(data)/sizeof(*data);
    for(unsigned i = 0; i < numOfFilts; i++) {
        data = filters.at(i).process(data, size);
    }
}
static GstPadProbeReturn
get_data (GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    GstMapInfo map;
    GstBuffer *buffer;

    buffer = GST_PAD_PROBE_INFO_BUFFER (info);

    buffer = gst_buffer_make_writable (buffer);

    if(buffer == NULL) return GST_PAD_PROBE_OK;

    if(gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
        data = (double*) map.data;
        process();
        gst_buffer_unmap (buffer, &map);
    }
    GST_PAD_PROBE_INFO_DATA (info) = buffer;

    return GST_PAD_PROBE_OK;
}

int main(int argc, char* argv[]) {
    GMainLoop *loop;
    GstElement *pipeline, *src, *resampler, *caps, *sink;
    GstCaps* filter;
    GstPad* pad;

    Config cfg;

    try {
        if(argc > 1) {
            cfg.readFile(argv[1]);
        } else {
            cfg.readFile(DEFAULT_NAME);
        }
    }
    catch(const FileIOException &fioex)
    {
      std::cerr << "I/O error while reading file." << std::endl;
      return(EXIT_FAILURE);
    }
    catch(const ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
      return(EXIT_FAILURE);
    }


    gst_init(&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    pipeline = gst_pipeline_new ("EQ");
    src = gst_element_factory_make("autoaudiosrc", "src");
    if(src == NULL) {
        g_error ("[GST] Can't create autoaudiosrc element");
    }

    resampler = gst_element_factory_make("audioresample", "resampler");
    if(resampler == NULL) {
        g_error ("[GST] Can't create audioresample element");
    }

    caps = gst_element_factory_make("capsfilter", "caps");
    g_assert(caps != NULL);


    sink = gst_element_factory_make("autoaudiosink", "sink");
    if(sink == NULL) {
        g_error ("[GST] Can't create autoaudiosink element");
    }

    gst_bin_add_many (GST_BIN (pipeline), src, resampler, caps, sink, NULL);
    gst_element_link_many (src, resampler, caps, sink, NULL);
    filter = gst_caps_new_simple ("audio/x-raw",
    "format", G_TYPE_STRING, "F32LE",
    "rate", G_TYPE_INT, "48000", NULL);
    g_object_set (G_OBJECT (caps), "caps", filter, NULL);
    gst_caps_unref(filter);

    pad = gst_element_get_static_pad (sink, "sink");
    gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback) get_data, NULL, NULL);
    gst_object_unref(pad);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if(gst_element_get_state (pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
        g_error ("Failed to go into PLAYING state");
    }
    cout << "Running..." << endl;
    g_main_loop_run (loop);

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}