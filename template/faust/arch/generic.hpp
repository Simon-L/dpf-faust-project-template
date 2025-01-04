{% block HeaderDescription %}
//------------------------------------------------------------------------------
// This file was generated using the Faust compiler (https://faust.grame.fr),
// and the Faust post-processor (https://github.com/SpotlightKid/faustdoctor).
//
// Source: {{filename}}
// Name: {{name}}
// Author: {{author}}
// Copyright: {{copyright}}
// License: {{license}}
// Version: {{version}}
// FAUST version: {{faustversion}}
// FAUST compilation options: {{meta.compile_options}}
//------------------------------------------------------------------------------
{% endblock %}

{% block HeaderPrologue %}
{% if fdrversioninfo is undefined or fdrversioninfo < (0, 2, 0) %}
{{fail("This template is not compatible with faustdoctor version < 0.2.0.")}}
{% endif %}
{% if not (Identifier is defined and
           Identifier == cid(Identifier)) %}
{{fail("`Identifier` is undefined or invalid.")}}
{% endif %}
{% endblock %}

#pragma once
#ifndef {{Identifier}}_Faust_pp_Gen_HPP_
#define {{Identifier}}_Faust_pp_Gen_HPP_

#include <memory>
#include <string>
#include <iostream>

#include "faust/dsp/dsp.h"
#include "faust/dsp/poly-dsp.h"
#include "faust/gui/meta.h"
#include "faust/gui/UI.h"
#include "faust/midi/midi.h"
#include "faust/gui/MidiUI.h"
#include "faust/gui/APIUI.h"

struct DistrhoMidiEvent {
    static constexpr const uint32_t kDataSize = 4;
    uint32_t frame;
    uint32_t size;
    uint8_t        data[kDataSize];
    const uint8_t* dataExt;
};

class dpf_midi : public midi_handler {
public:
    
    void DumpEvent(const DistrhoMidiEvent* event)
    {
        uint8_t b0 = event->data[0]; // status + channel
        uint8_t b1 = event->data[1]; // note
        uint8_t b2 = event->data[2]; // velocity
        printf("\tMIDI in 0x%x %d %d\n", b0, b1, b2);
    }
    
    bool FindEvent(const DistrhoMidiEvent ev, const DistrhoMidiEvent* midiEvents, uint32_t midiEventsCount)
    {
        for (size_t i = 0; i < midiEventsCount; i++) {
            if ((ev.data[0] == midiEvents[i].data[0]) && (ev.data[1] == midiEvents[i].data[1]) && (ev.data[2] == midiEvents[i].data[2]))
            {
                return true;
            }
        }
        return false;
    }
    
    void processMidiInBuffer(const DistrhoMidiEvent* midiEvents, uint32_t midiEventsCount)
    {
        DistrhoMidiEvent processed[midiEventsCount];
        for (size_t m = 0; m < midiEventsCount; ++m) {

            DistrhoMidiEvent event = midiEvents[m];
            size_t nBytes = event.size;
            int type = (int)event.data[0] & 0xf0;
            int channel = (int)event.data[0] & 0x0f;
            double time = event.frame; // Timestamp in frames
            
            // DumpEvent(&event);
            
            bool skip = false;
            
            if ((m > 0) && (FindEvent(event, processed, m)))
            {
                skip = true;
            } else if ((midiEventsCount > 1) && (type == 0x90)) {
                DistrhoMidiEvent event_off = event;
                event_off.data[0] = channel | 0x80;
                if (FindEvent(event_off, midiEvents, midiEventsCount))
                {
                    skip = true;
                }
            }
            
            if (!skip) {
                // MIDI sync
                if (nBytes == 1) {
                    // handleSync(time, (int)event.data[0]);
                } else if (nBytes == 2) {
                    // handleData1(time, type, channel, (int)event.data[1]);
                } else if (nBytes == 3) {
                    handleData2(time, type, channel, (int)event.data[1], (int)event.data[2]);
                } else {
                    // std::vector<unsigned char> message(event.data, event.data + event.size);
                    // handleMessage(time, type, message);
                }
                processed[m] = event;
            }
        }
    }

    dpf_midi(const std::string& name = "DPFHandler")
        :midi_handler(name)
    {
    }
    virtual ~dpf_midi()
    {
    }
};



class {{Identifier}} {
public:
    {{Identifier}}();
    ~{{Identifier}}();

    void init(float sample_rate);
    void clear() noexcept;

    void process(
        {%+ for i in range(inputs) %}const float *in{{i}}, {% endfor +%}
        {%+ for i in range(outputs) %}float *out{{i}}, {% endfor +%}
        unsigned count) noexcept;
        
    void processMidi(const DistrhoMidiEvent* midiEvents, uint32_t midiEventsCount);

    enum { NumInputs = {{inputs}} };
    enum { NumOutputs = {{outputs}} };
    enum { NumActives = {{active|length}} };
    enum { NumPassives = {{passive|length}} };
    enum { NumParameters = {{active|length + passive|length}} };
    enum { NumParametersPoly = {{active|length + passive|length + 1}} };

    enum Parameter {
        {% for w in active + passive %}
        p_{{w.symbol}},
        {% endfor %}
    };

    struct ParameterRange {
        float init;
        float min;
        float max;
    };

    struct ParameterScalePoint {
        std::string label;
        float value;
    };

    int parameter_group(unsigned index) noexcept;
    int parameter_order(unsigned index) noexcept;
    static const char *parameter_group_label(unsigned group_id) noexcept;
    static const char *parameter_group_symbol(unsigned group_id) noexcept;
    static const char *parameter_label(unsigned index) noexcept;
    static const char *parameter_short_label(unsigned index) noexcept;
    static const char *parameter_description(unsigned index) noexcept;
    static const char *parameter_style(unsigned index) noexcept;
    static const char *parameter_symbol(unsigned index) noexcept;
    static const char *parameter_unit(unsigned index) noexcept;
    static const ParameterRange *parameter_range(unsigned index) noexcept;
    unsigned parameter_scale_point_count(unsigned index) noexcept;
    static const ParameterScalePoint *parameter_scale_point(unsigned index, unsigned point) noexcept;
    static bool parameter_is_trigger(unsigned index) noexcept;
    static bool parameter_is_boolean(unsigned index) noexcept;
    static bool parameter_is_enum(unsigned index) noexcept;
    static bool parameter_is_integer(unsigned index) noexcept;
    static bool parameter_is_logarithmic(unsigned index) noexcept;

    float get_parameter(unsigned index) const noexcept;
    void set_parameter(unsigned index, float value) noexcept;
    
    {% for w in active + passive %}
    float get_{{w.symbol}}() const noexcept;
    {% endfor %}
    {% for w in active %}
    void set_{{w.symbol}}(float value) noexcept;
    {% endfor %}

public:
    // class BasicDsp;

private:
    std::unique_ptr<mydsp_poly> fDsp;
    dpf_midi* dpf_midi_handler;
    MidiUI* midi_interface;
    APIUI* api;

{% block ClassExtraDecls %}
{% endblock %}
};

{% block HeaderEpilogue %}
{% endblock %}

#endif // {{Identifier}}_Faust_pp_Gen_HPP_
