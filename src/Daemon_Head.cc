#include <libconfig.h++>
#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/gstcaps.h>
#include "Filter.hh"
#include <iostream>
#define DEFAULT_NAME "eq.cfg"
using namespace std;
using namespace libconfig;

struct CustomData {
    GMainLoop* loop;
    GstElement* pipeline;
    GstElement *src,*resample,*caps,*sink;
    GstCaps *filt;
    GstPad* pad;
};
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
    struct CustomData data;

    Config cfg;
/*
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

*/
    gst_init(&argc, &argv);
    data.loop = g_main_loop_new(NULL, false);

    data.pipeline = gst_pipeline_new("equalizing");
    data.src = gst_element_factory_make("autoaudiosrc", "src");
    if(data.src == NULL) {
        g_error ("Can't create autoaudiosrc element");
    }

    data.resample = gst_element_factory_make("audioresample", "resample");
    if(data.resample == NULL) {
        g_error ("Can't create audioresample element");
    }
    
    data.caps = gst_element_factory_make("capsfilter", "filt");
    g_assert (data.caps != NULL);

    data.sink = gst_element_factory_make("autoaudiosink", "sink");
    if(data.sink == NULL) {
        g_error ("Can't create autoaudiosink element");
    }
    cout << "Linking..." << endl;

    gst_bin_add_many (GST_BIN(data.pipeline), data.src, data.resample, data.caps, data.sink, NULL);
    gst_element_link_many (data.src,data.resample,data.caps,data.sink, NULL);
    data.filt = gst_caps_new_simple ("audio/x-raw",
    "rate", G_TYPE_INT, 48000, NULL);
    g_object_set (G_OBJECT (data.caps), "caps", data.filt, NULL);
    gst_caps_unref(data.filt);

    data.pad = gst_element_get_static_pad(data.sink, "sink");
    gst_pad_add_probe (data.pad, GST_PAD_PROBE_TYPE_BUFFER,
    (GstPadProbeCallback) get_data, NULL, NULL);
    gst_object_unref (data.pad);

    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);

    if (gst_element_get_state (data.pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) g_error ("Failed to go in PLAYING state");

    cout << "Running..." << endl;
    g_main_loop_run(data.loop);

    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);
    return 0;
}