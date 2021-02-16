#define DEFAULT_NAME "eq.cfg"
#define VERSION "0.1(testing)"

#include <libconfig.h++>
#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/gstcaps.h>
#include "Filter.hh"
#include <iostream>
#include <cstring>

using namespace std;
using namespace libconfig;

struct CustomData {
    GMainLoop* loop;
    GstElement* pipeline;
    GstElement *src,*resample,*caps,*sink;
    GstCaps *filt;
    GstPad* pad;
};
static double* d;

inline void process() {
    auto it = filters.begin();
    for(;it < filters.end();it++) {
        for(unsigned i = 0; i < sizeof(d)/sizeof(*d);i++) {
            *(d + i) = it->process(*(d + i));
        }
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
        //d = (double*)malloc(map.size);
        //memcpy((double*)d, map.data, map.size);
        d = (double*) map.data;
        process();
        //memcpy(map.data, d, map.size);
        gst_buffer_unmap (buffer, &map);
    }
    GST_PAD_PROBE_INFO_DATA (info) = buffer;

    return GST_PAD_PROBE_OK;
}

int main(int argc, char* argv[]) {

    if(argv[1] == "-h" || argv[1] == "--help") {
        cout << "ra1nst0rm3d's SimpleEQ version " << VERSION << endl <<
    }
    struct CustomData data;

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

    const Setting& root = cfg.getRoot();

    try {
        const Setting& filts = root["filters"];
        numOfFilts = root.getLength();

        int freq,gain,type;
        double Q;

        
        for(unsigned i = 0; i < numOfFilts; i++) {
            const Setting& f = filts[i];

            if(!(f.lookupValue("freq", freq) &&
                 f.lookupValue("gain", gain) &&
                 f.lookupValue("Q", Q) &&
                 f.lookupValue("filter_type", type))) continue;
            
            filters.clear();

            if(type == 0) {
                Filter fi(freq,gain,HIGH_SHELF, 48000, Q);
                filters.push_back(fi);
            } else {
                Filter fi(freq,gain, LOW_SHELF, 48000, Q);
                filters.push_back(fi);
            }

        }
    }
    catch(const SettingNotFoundException &nfex)
  {
    /*cerr << "Some setting not found, recheck config file..." << endl;
    return 0;*/
  }

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

    data.pad = gst_element_get_static_pad(data.src, "src");
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