/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class RnboYoi : public PatcherInterfaceImpl {
public:

RnboYoi()
{
}

~RnboYoi()
{
}

RnboYoi* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, -1468824490, false);
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

number samplerate() {
    return this->sr;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

Index vectorsize() {
    return this->vs;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 1;
}

void processMidiEvent(MillisecondTime time, int port, ConstByteArray data, Index length) {
    this->updateTime(time);
    this->midiin_01_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    RNBO_UNUSED(numInputs);
    RNBO_UNUSED(inputs);
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    this->gen_01_perform(this->gen_01_in1, this->signals[0], n);
    this->limi_04_perform(this->signals[0], this->signals[1], n);

    this->svf_tilde_01_perform(
        this->signals[1],
        this->svf_tilde_01_freq,
        this->svf_tilde_01_q,
        this->dummyBuffer,
        this->dummyBuffer,
        this->signals[0],
        this->dummyBuffer,
        n
    );

    this->limi_02_perform(this->signals[0], this->signals[1], n);
    this->downsamp_tilde_01_perform(this->signals[1], this->downsamp_tilde_01_downSample, this->signals[0], n);
    this->limi_03_perform(this->signals[0], this->signals[1], n);

    this->adsr_01_perform(
        this->adsr_01_attack,
        this->adsr_01_decay,
        this->adsr_01_sustain,
        this->adsr_01_release,
        this->zeroBuffer,
        this->signals[0],
        n
    );

    this->dspexpr_01_perform(this->signals[1], this->signals[0], this->signals[2], n);
    this->limi_01_perform(this->signals[2], out1, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 3; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->adsr_01_triggerBuf = resizeSignal(this->adsr_01_triggerBuf, this->maxvs, maxBlockSize);
        this->adsr_01_triggerValueBuf = resizeSignal(this->adsr_01_triggerValueBuf, this->maxvs, maxBlockSize);
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->gen_01_dspsetup(forceDSPSetup);
    this->limi_04_dspsetup(forceDSPSetup);
    this->svf_tilde_01_dspsetup(forceDSPSetup);
    this->limi_02_dspsetup(forceDSPSetup);
    this->limi_03_dspsetup(forceDSPSetup);
    this->adsr_01_dspsetup(forceDSPSetup);
    this->limi_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->RNBODefaultMtofLookupTable256);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 1;
}

void fillRNBODefaultMtofLookupTable256(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        number midivalue = -256. + (number)512. / (bufsize - 1) * i;
        buffer[i] = rnbo_exp(.057762265 * (midivalue - 69.0));
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 0:
        {
        this->fillRNBODefaultMtofLookupTable256(ref);
        break;
        }
    }
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->mtof_01_innerMtoF_buffer = new Float64Buffer(this->RNBODefaultMtofLookupTable256);
    }
}

void initialize() {
    this->RNBODefaultMtofLookupTable256 = initDataRef("RNBODefaultMtofLookupTable256", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->RNBODefaultMtofLookupTable256->setIndex(0);
    this->mtof_01_innerMtoF_buffer = new Float64Buffer(this->RNBODefaultMtofLookupTable256);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "downsample"));
    this->param_02_getPresetValue(getSubState(preset, "cutoff"));
    this->param_03_getPresetValue(getSubState(preset, "resonant"));
    this->param_04_getPresetValue(getSubState(preset, "attack"));
    this->param_05_getPresetValue(getSubState(preset, "decay"));
    this->param_06_getPresetValue(getSubState(preset, "sustain"));
    this->param_07_getPresetValue(getSubState(preset, "release"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "downsample"));
    this->param_02_setPresetValue(getSubState(preset, "cutoff"));
    this->param_03_setPresetValue(getSubState(preset, "resonant"));
    this->param_04_setPresetValue(getSubState(preset, "attack"));
    this->param_05_setPresetValue(getSubState(preset, "decay"));
    this->param_06_setPresetValue(getSubState(preset, "sustain"));
    this->param_07_setPresetValue(getSubState(preset, "release"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 7;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "downsample";
        }
    case 1:
        {
        return "cutoff";
        }
    case 2:
        {
        return "resonant";
        }
    case 3:
        {
        return "attack";
        }
    case 4:
        {
        return "decay";
        }
    case 5:
        {
        return "sustain";
        }
    case 6:
        {
        return "release";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "downsample";
        }
    case 1:
        {
        return "cutoff";
        }
    case 2:
        {
        return "resonant";
        }
    case 3:
        {
        return "attack";
        }
    case 4:
        {
        return "decay";
        }
    case 5:
        {
        return "sustain";
        }
    case 6:
        {
        return "release";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 17;
            info->min = 0;
            info->max = 30;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 500;
            info->min = 0;
            info->max = 5000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 10000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 500;
            info->min = 0;
            info->max = 10000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 10000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 2:
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 30 ? 30 : value));
            ParameterValue normalizedValue = (value - 0) / (30 - 0);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 5000 ? 5000 : value));
            ParameterValue normalizedValue = (value - 0) / (5000 - 0);
            return normalizedValue;
        }
        }
    case 3:
    case 4:
    case 6:
        {
        {
            value = (value < 0 ? 0 : (value > 10000 ? 10000 : value));
            ParameterValue normalizedValue = (value - 0) / (10000 - 0);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 2:
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (30 - 0);
            }
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (5000 - 0);
            }
        }
        }
    case 3:
    case 4:
    case 6:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (10000 - 0);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime time, ClockId index, bool hasValue, ParameterValue value) {
    RNBO_UNUSED(value);
    RNBO_UNUSED(hasValue);
    this->updateTime(time);

    switch (index) {
    case -1468824490:
        {
        this->adsr_01_mute_bang();
        break;
        }
    }
}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->downsamp_tilde_01_downSample_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->svf_tilde_01_freq_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->svf_tilde_01_q_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->adsr_01_attack_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->adsr_01_decay_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->adsr_01_sustain_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->adsr_01_release_set(v);
}

void adsr_01_mute_bang() {}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 0;
}

Index getNumOutputChannels() const {
    return 1;
}

void allocateDataRefs() {
    this->mtof_01_innerMtoF_buffer->requestSize(65536, 1);
    this->mtof_01_innerMtoF_buffer->setSampleRate(this->sr);
    this->mtof_01_innerMtoF_buffer = this->mtof_01_innerMtoF_buffer->allocateIfNeeded();

    if (this->RNBODefaultMtofLookupTable256->hasRequestedSize()) {
        if (this->RNBODefaultMtofLookupTable256->wantsFill())
            this->fillRNBODefaultMtofLookupTable256(this->RNBODefaultMtofLookupTable256);

        this->getEngine()->sendDataRefUpdated(0);
    }
}

void initializeObjects() {
    this->mtof_01_innerScala_init();
    this->mtof_01_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 30 ? 30 : (v < 0 ? 0 : v));
    return v;
}

void downsamp_tilde_01_downSample_set(number v) {
    this->downsamp_tilde_01_downSample = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 5000 ? 5000 : (v < 0 ? 0 : v));
    return v;
}

void svf_tilde_01_freq_set(number v) {
    this->svf_tilde_01_freq = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void svf_tilde_01_q_set(number v) {
    this->svf_tilde_01_q = v;
}

static number param_04_value_constrain(number v) {
    v = (v > 10000 ? 10000 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_attack_set(number v) {
    this->adsr_01_attack = v;
}

static number param_05_value_constrain(number v) {
    v = (v > 10000 ? 10000 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_decay_set(number v) {
    this->adsr_01_decay = v;
}

static number param_06_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_sustain_set(number v) {
    this->adsr_01_sustain = v;
}

static number param_07_value_constrain(number v) {
    v = (v > 10000 ? 10000 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_release_set(number v) {
    this->adsr_01_release = v;
}

void midiparse_01_channel_set(number ) {}

void adsr_01_trigger_number_set(number v) {
    this->adsr_01_trigger_number = v;

    if (v != 0)
        this->adsr_01_triggerBuf[(Index)this->sampleOffsetIntoNextAudioBuffer] = 1;

    for (number i = this->sampleOffsetIntoNextAudioBuffer; i < this->vectorsize(); i++) {
        this->adsr_01_triggerValueBuf[(Index)i] = v;
    }
}

void expr_01_out1_set(number v) {
    this->expr_01_out1 = v;
    this->adsr_01_trigger_number_set(this->expr_01_out1);
}

void expr_01_in1_set(number in1) {
    this->expr_01_in1 = in1;

    this->expr_01_out1_set(
        (this->expr_01_in2 == 0 ? 0 : (this->expr_01_in2 == 0. ? 0. : this->expr_01_in1 / this->expr_01_in2))
    );//#map:/_obj-12:1
}

void unpack_01_out2_set(number v) {
    this->unpack_01_out2 = v;
    this->expr_01_in1_set(v);
}

void gen_01_in1_set(number v) {
    this->gen_01_in1 = v;
}

void mtof_01_out_set(const list& v) {
    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->gen_01_in1_set(converted);
    }
}

void mtof_01_midivalue_set(const list& v) {
    this->mtof_01_midivalue = jsCreateListCopy(v);
    list tmp = list();

    for (int i = 0; i < this->mtof_01_midivalue->length; i++) {
        tmp->push(
            this->mtof_01_innerMtoF_next(this->mtof_01_midivalue[(Index)i], this->mtof_01_base)
        );
    }

    this->mtof_01_out_set(tmp);
}

void unpack_01_out1_set(number v) {
    this->unpack_01_out1 = v;

    {
        list converted = {v};
        this->mtof_01_midivalue_set(converted);
    }
}

void unpack_01_input_list_set(const list& v) {
    if (v->length > 1)
        this->unpack_01_out2_set(v[1]);

    if (v->length > 0)
        this->unpack_01_out1_set(v[0]);
}

void midiparse_01_noteonoroff_set(const list& v) {
    this->unpack_01_input_list_set(v);
}

void midiparse_01_polypressure_set(const list& ) {}

void midiparse_01_controlchange_set(const list& ) {}

void midiparse_01_programchange_set(number ) {}

void midiparse_01_aftertouch_set(number ) {}

void midiparse_01_pitchbend_set(number ) {}

void midiparse_01_midiin_set(number midivalue) {
    list result = this->midiparse_01_parser_next(midivalue);

    switch ((int)result[0]) {
    case 0:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_noteonoroff_set({result[1], result[2]});
        break;
        }
    case 1:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_polypressure_set({result[1], result[2]});
        break;
        }
    case 2:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_controlchange_set({result[1], result[2]});
        break;
        }
    case 3:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_programchange_set(result[1]);
        break;
        }
    case 4:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_aftertouch_set(result[1]);
        break;
        }
    case 5:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_pitchbend_set(result[1]);
        break;
        }
    }
}

void midiin_01_midiout_set(number v) {
    this->midiparse_01_midiin_set(v);
}

void midiin_01_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(port);
    RNBO_UNUSED(channel);
    RNBO_UNUSED(status);
    Index i;

    for (i = 0; i < length; i++) {
        this->midiin_01_midiout_set(data[i]);
    }
}

void gen_01_perform(number in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        number phasor_1_1 = this->gen_01_phasor_0_next(in1, 0);
        number gte_2_2 = phasor_1_1 >= 0.5;
        out1[(Index)i] = gte_2_2;
    }
}

void limi_04_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input1);
    auto __limi_04_lookaheadInv = this->limi_04_lookaheadInv;
    auto __limi_04_threshold = this->limi_04_threshold;
    auto __limi_04_lookahead = this->limi_04_lookahead;
    auto __limi_04_recover = this->limi_04_recover;
    auto __limi_04_last = this->limi_04_last;
    auto __limi_04_postamp = this->limi_04_postamp;
    auto __limi_04_lookaheadIndex = this->limi_04_lookaheadIndex;
    auto __limi_04_preamp = this->limi_04_preamp;
    auto __limi_04_dcblock = this->limi_04_dcblock;
    auto __limi_04_bypass = this->limi_04_bypass;
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_04_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_04_dcblock) ? this->limi_04_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_04_preamp;
                this->limi_04_lookaheadBuffers[(Index)j][__limi_04_lookaheadIndex] = v * __limi_04_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_04_last > 0.01)
                    v = __limi_04_last + __limi_04_recover * __limi_04_last;
                else
                    v = __limi_04_last + __limi_04_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_04_gainBuffer[__limi_04_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_04_lookaheadIndex - (int)(__limi_04_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_04_lookahead);

            if (hotSample * v > __limi_04_threshold) {
                number newgain;
                number curgain = __limi_04_threshold / hotSample;
                number inc = __limi_04_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_04_lookahead); j++) {
                    int k = (int)(__limi_04_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_04_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_04_gainBuffer[(Index)k])
                        this->limi_04_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_04_lookaheadInv;
                }
            }

            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = this->limi_04_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_04_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_04_last = this->limi_04_gainBuffer[__limi_04_lookaheadIndex];
            __limi_04_lookaheadIndex++;

            if (__limi_04_lookaheadIndex >= __limi_04_lookahead)
                __limi_04_lookaheadIndex = 0;
        }
    }

    this->limi_04_lookaheadIndex = __limi_04_lookaheadIndex;
    this->limi_04_last = __limi_04_last;
}

void svf_tilde_01_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    auto __svf_tilde_01_high = this->svf_tilde_01_high;
    auto __svf_tilde_01_notch = this->svf_tilde_01_notch;
    auto __svf_tilde_01_band = this->svf_tilde_01_band;
    auto __svf_tilde_01_low = this->svf_tilde_01_low;
    auto __svf_tilde_01__q = this->svf_tilde_01__q;
    auto __svf_tilde_01__freq = this->svf_tilde_01__freq;
    Index i;

    for (i = 0; i < n; i++) {
        if (this->svf_tilde_01_dFreq_next(freq) != 0) {
            __svf_tilde_01__freq = (freq > this->sr * 0.25 ? this->sr * 0.25 : (freq < 0.0 ? 0.0 : freq));
            __svf_tilde_01__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_01__freq / this->sr);
            __svf_tilde_01__freq = (__svf_tilde_01__freq > 0.99 ? 0.99 : (__svf_tilde_01__freq < 0.001 ? 0.001 : __svf_tilde_01__freq));
        }

        if (this->svf_tilde_01_dQ_next(q) != 0) {
            __svf_tilde_01__q = (1. - q) * 1.2;
            __svf_tilde_01__q = __svf_tilde_01__q * __svf_tilde_01__q;
            __svf_tilde_01__q = (__svf_tilde_01__q > 0.99 ? 0.99 : (__svf_tilde_01__q < 0.001 ? 0.001 : __svf_tilde_01__q));
        }

        __svf_tilde_01_low += __svf_tilde_01__freq * __svf_tilde_01_band;
        __svf_tilde_01_notch = x[(Index)i] - __svf_tilde_01__q * __svf_tilde_01_band;
        __svf_tilde_01_high = __svf_tilde_01_notch - __svf_tilde_01_low;
        __svf_tilde_01_band += __svf_tilde_01__freq * __svf_tilde_01_high - __svf_tilde_01_band * __svf_tilde_01_band * __svf_tilde_01_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_01_low;
        tmp[1] = __svf_tilde_01_high;
        tmp[2] = __svf_tilde_01_band;
        tmp[3] = __svf_tilde_01_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_01__freq = __svf_tilde_01__freq;
    this->svf_tilde_01__q = __svf_tilde_01__q;
    this->svf_tilde_01_low = __svf_tilde_01_low;
    this->svf_tilde_01_band = __svf_tilde_01_band;
    this->svf_tilde_01_notch = __svf_tilde_01_notch;
    this->svf_tilde_01_high = __svf_tilde_01_high;
}

void limi_02_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input1);
    auto __limi_02_lookaheadInv = this->limi_02_lookaheadInv;
    auto __limi_02_threshold = this->limi_02_threshold;
    auto __limi_02_lookahead = this->limi_02_lookahead;
    auto __limi_02_recover = this->limi_02_recover;
    auto __limi_02_last = this->limi_02_last;
    auto __limi_02_postamp = this->limi_02_postamp;
    auto __limi_02_lookaheadIndex = this->limi_02_lookaheadIndex;
    auto __limi_02_preamp = this->limi_02_preamp;
    auto __limi_02_dcblock = this->limi_02_dcblock;
    auto __limi_02_bypass = this->limi_02_bypass;
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_02_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_02_dcblock) ? this->limi_02_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_02_preamp;
                this->limi_02_lookaheadBuffers[(Index)j][__limi_02_lookaheadIndex] = v * __limi_02_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_02_last > 0.01)
                    v = __limi_02_last + __limi_02_recover * __limi_02_last;
                else
                    v = __limi_02_last + __limi_02_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_02_gainBuffer[__limi_02_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_02_lookaheadIndex - (int)(__limi_02_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_02_lookahead);

            if (hotSample * v > __limi_02_threshold) {
                number newgain;
                number curgain = __limi_02_threshold / hotSample;
                number inc = __limi_02_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_02_lookahead); j++) {
                    int k = (int)(__limi_02_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_02_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_02_gainBuffer[(Index)k])
                        this->limi_02_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_02_lookaheadInv;
                }
            }

            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = this->limi_02_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_02_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_02_last = this->limi_02_gainBuffer[__limi_02_lookaheadIndex];
            __limi_02_lookaheadIndex++;

            if (__limi_02_lookaheadIndex >= __limi_02_lookahead)
                __limi_02_lookaheadIndex = 0;
        }
    }

    this->limi_02_lookaheadIndex = __limi_02_lookaheadIndex;
    this->limi_02_last = __limi_02_last;
}

void downsamp_tilde_01_perform(const Sample * x, number downSample, SampleValue * out1, Index n) {
    auto __downsamp_tilde_01_hold = this->downsamp_tilde_01_hold;
    auto __downsamp_tilde_01_current = this->downsamp_tilde_01_current;
    auto __downsamp_tilde_01_lastDownSample = this->downsamp_tilde_01_lastDownSample;
    Index i;

    for (i = 0; i < n; i++) {
        if (downSample != __downsamp_tilde_01_lastDownSample) {
            __downsamp_tilde_01_lastDownSample = this->maximum(1.0, rnbo_abs(downSample));

            if (__downsamp_tilde_01_lastDownSample < __downsamp_tilde_01_current) {
                __downsamp_tilde_01_current = __downsamp_tilde_01_lastDownSample;
            }
        }

        __downsamp_tilde_01_current++;

        if (__downsamp_tilde_01_current >= __downsamp_tilde_01_lastDownSample) {
            __downsamp_tilde_01_current -= __downsamp_tilde_01_lastDownSample;
            __downsamp_tilde_01_hold = x[(Index)i];
        }

        out1[(Index)i] = __downsamp_tilde_01_hold;
    }

    this->downsamp_tilde_01_lastDownSample = __downsamp_tilde_01_lastDownSample;
    this->downsamp_tilde_01_current = __downsamp_tilde_01_current;
    this->downsamp_tilde_01_hold = __downsamp_tilde_01_hold;
}

void limi_03_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input1);
    auto __limi_03_lookaheadInv = this->limi_03_lookaheadInv;
    auto __limi_03_threshold = this->limi_03_threshold;
    auto __limi_03_lookahead = this->limi_03_lookahead;
    auto __limi_03_recover = this->limi_03_recover;
    auto __limi_03_last = this->limi_03_last;
    auto __limi_03_postamp = this->limi_03_postamp;
    auto __limi_03_lookaheadIndex = this->limi_03_lookaheadIndex;
    auto __limi_03_preamp = this->limi_03_preamp;
    auto __limi_03_dcblock = this->limi_03_dcblock;
    auto __limi_03_bypass = this->limi_03_bypass;
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_03_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_03_dcblock) ? this->limi_03_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_03_preamp;
                this->limi_03_lookaheadBuffers[(Index)j][__limi_03_lookaheadIndex] = v * __limi_03_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_03_last > 0.01)
                    v = __limi_03_last + __limi_03_recover * __limi_03_last;
                else
                    v = __limi_03_last + __limi_03_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_03_gainBuffer[__limi_03_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_03_lookaheadIndex - (int)(__limi_03_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_03_lookahead);

            if (hotSample * v > __limi_03_threshold) {
                number newgain;
                number curgain = __limi_03_threshold / hotSample;
                number inc = __limi_03_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_03_lookahead); j++) {
                    int k = (int)(__limi_03_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_03_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_03_gainBuffer[(Index)k])
                        this->limi_03_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_03_lookaheadInv;
                }
            }

            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = this->limi_03_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_03_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_03_last = this->limi_03_gainBuffer[__limi_03_lookaheadIndex];
            __limi_03_lookaheadIndex++;

            if (__limi_03_lookaheadIndex >= __limi_03_lookahead)
                __limi_03_lookaheadIndex = 0;
        }
    }

    this->limi_03_lookaheadIndex = __limi_03_lookaheadIndex;
    this->limi_03_last = __limi_03_last;
}

void adsr_01_perform(
    number attack,
    number decay,
    number sustain,
    number release,
    const SampleValue * trigger_signal,
    SampleValue * out,
    Index n
) {
    RNBO_UNUSED(trigger_signal);
    auto __adsr_01_trigger_number = this->adsr_01_trigger_number;
    auto __adsr_01_time = this->adsr_01_time;
    auto __adsr_01_amplitude = this->adsr_01_amplitude;
    auto __adsr_01_outval = this->adsr_01_outval;
    auto __adsr_01_startingpoint = this->adsr_01_startingpoint;
    auto __adsr_01_phase = this->adsr_01_phase;
    auto __adsr_01_legato = this->adsr_01_legato;
    auto __adsr_01_lastTriggerVal = this->adsr_01_lastTriggerVal;
    auto __adsr_01_mspersamp = this->adsr_01_mspersamp;
    bool bangMute = false;

    for (Index i = 0; i < n; i++) {
        number clampedattack = (attack > __adsr_01_mspersamp ? attack : __adsr_01_mspersamp);
        number clampeddecay = (decay > __adsr_01_mspersamp ? decay : __adsr_01_mspersamp);
        number clampedrelease = (release > __adsr_01_mspersamp ? release : __adsr_01_mspersamp);
        number currentTriggerVal = this->adsr_01_triggerValueBuf[(Index)i];

        if ((__adsr_01_lastTriggerVal == 0.0 && currentTriggerVal != 0.0) || this->adsr_01_triggerBuf[(Index)i] == 1) {
            if ((bool)(__adsr_01_legato)) {
                if (__adsr_01_phase != 0) {
                    __adsr_01_startingpoint = __adsr_01_outval;
                } else {
                    __adsr_01_startingpoint = 0;
                }
            } else {
                __adsr_01_startingpoint = 0;
            }

            __adsr_01_amplitude = currentTriggerVal;
            __adsr_01_phase = 1;
            __adsr_01_time = 0.0;
            bangMute = false;
        } else if (__adsr_01_lastTriggerVal != 0.0 && currentTriggerVal == 0.0) {
            if (__adsr_01_phase != 4 && __adsr_01_phase != 0) {
                __adsr_01_phase = 4;
                __adsr_01_amplitude = __adsr_01_outval;
                __adsr_01_time = 0.0;
            }
        }

        __adsr_01_time += __adsr_01_mspersamp;

        if (__adsr_01_phase == 0) {
            __adsr_01_outval = 0;
        } else if (__adsr_01_phase == 1) {
            if (__adsr_01_time > clampedattack) {
                __adsr_01_time -= clampedattack;
                __adsr_01_phase = 2;
                __adsr_01_outval = __adsr_01_amplitude;
            } else {
                __adsr_01_outval = (__adsr_01_amplitude - __adsr_01_startingpoint) * __adsr_01_time / clampedattack + __adsr_01_startingpoint;
            }
        } else if (__adsr_01_phase == 2) {
            if (__adsr_01_time > clampeddecay) {
                __adsr_01_time -= clampeddecay;
                __adsr_01_phase = 3;
                __adsr_01_outval = __adsr_01_amplitude * sustain;
            } else {
                __adsr_01_outval = __adsr_01_amplitude * sustain + (__adsr_01_amplitude - __adsr_01_amplitude * sustain) * (1. - __adsr_01_time / clampeddecay);
            }
        } else if (__adsr_01_phase == 3) {
            __adsr_01_outval = __adsr_01_amplitude * sustain;
        } else if (__adsr_01_phase == 4) {
            if (__adsr_01_time > clampedrelease) {
                __adsr_01_time = 0;
                __adsr_01_phase = 0;
                __adsr_01_outval = 0;
                __adsr_01_amplitude = 0;
                bangMute = true;
            } else {
                __adsr_01_outval = __adsr_01_amplitude * (1.0 - __adsr_01_time / clampedrelease);
            }
        }

        out[(Index)i] = __adsr_01_outval;
        this->adsr_01_triggerBuf[(Index)i] = 0;
        this->adsr_01_triggerValueBuf[(Index)i] = __adsr_01_trigger_number;
        __adsr_01_lastTriggerVal = currentTriggerVal;
    }

    if ((bool)(bangMute)) {
        this->getEngine()->scheduleClockEventWithValue(
            this,
            -1468824490,
            this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
            0
        );;
    }

    this->adsr_01_lastTriggerVal = __adsr_01_lastTriggerVal;
    this->adsr_01_phase = __adsr_01_phase;
    this->adsr_01_startingpoint = __adsr_01_startingpoint;
    this->adsr_01_outval = __adsr_01_outval;
    this->adsr_01_amplitude = __adsr_01_amplitude;
    this->adsr_01_time = __adsr_01_time;
}

void dspexpr_01_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void limi_01_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input1);
    auto __limi_01_lookaheadInv = this->limi_01_lookaheadInv;
    auto __limi_01_threshold = this->limi_01_threshold;
    auto __limi_01_lookahead = this->limi_01_lookahead;
    auto __limi_01_recover = this->limi_01_recover;
    auto __limi_01_last = this->limi_01_last;
    auto __limi_01_postamp = this->limi_01_postamp;
    auto __limi_01_lookaheadIndex = this->limi_01_lookaheadIndex;
    auto __limi_01_preamp = this->limi_01_preamp;
    auto __limi_01_dcblock = this->limi_01_dcblock;
    auto __limi_01_bypass = this->limi_01_bypass;
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_01_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_01_dcblock) ? this->limi_01_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_01_preamp;
                this->limi_01_lookaheadBuffers[(Index)j][__limi_01_lookaheadIndex] = v * __limi_01_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_01_last > 0.01)
                    v = __limi_01_last + __limi_01_recover * __limi_01_last;
                else
                    v = __limi_01_last + __limi_01_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_01_gainBuffer[__limi_01_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_01_lookaheadIndex - (int)(__limi_01_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_01_lookahead);

            if (hotSample * v > __limi_01_threshold) {
                number newgain;
                number curgain = __limi_01_threshold / hotSample;
                number inc = __limi_01_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_01_lookahead); j++) {
                    int k = (int)(__limi_01_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_01_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_01_gainBuffer[(Index)k])
                        this->limi_01_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_01_lookaheadInv;
                }
            }

            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = this->limi_01_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_01_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_01_last = this->limi_01_gainBuffer[__limi_01_lookaheadIndex];
            __limi_01_lookaheadIndex++;

            if (__limi_01_lookaheadIndex >= __limi_01_lookahead)
                __limi_01_lookaheadIndex = 0;
        }
    }

    this->limi_01_lookaheadIndex = __limi_01_lookaheadIndex;
    this->limi_01_last = __limi_01_last;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void limi_01_lookahead_setter(number v) {
    this->limi_01_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_01_lookaheadInv = (number)1 / this->limi_01_lookahead;
}

void limi_01_preamp_setter(number v) {
    this->limi_01_preamp = rnbo_pow(10., v * 0.05);
}

void limi_01_postamp_setter(number v) {
    this->limi_01_postamp = rnbo_pow(10., v * 0.05);
}

void limi_01_threshold_setter(number v) {
    this->limi_01_threshold = rnbo_pow(10., v * 0.05);
}

void limi_02_lookahead_setter(number v) {
    this->limi_02_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_02_lookaheadInv = (number)1 / this->limi_02_lookahead;
}

void limi_02_preamp_setter(number v) {
    this->limi_02_preamp = rnbo_pow(10., v * 0.05);
}

void limi_02_postamp_setter(number v) {
    this->limi_02_postamp = rnbo_pow(10., v * 0.05);
}

void limi_02_threshold_setter(number v) {
    this->limi_02_threshold = rnbo_pow(10., v * 0.05);
}

void limi_03_lookahead_setter(number v) {
    this->limi_03_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_03_lookaheadInv = (number)1 / this->limi_03_lookahead;
}

void limi_03_preamp_setter(number v) {
    this->limi_03_preamp = rnbo_pow(10., v * 0.05);
}

void limi_03_postamp_setter(number v) {
    this->limi_03_postamp = rnbo_pow(10., v * 0.05);
}

void limi_03_threshold_setter(number v) {
    this->limi_03_threshold = rnbo_pow(10., v * 0.05);
}

void limi_04_lookahead_setter(number v) {
    this->limi_04_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_04_lookaheadInv = (number)1 / this->limi_04_lookahead;
}

void limi_04_preamp_setter(number v) {
    this->limi_04_preamp = rnbo_pow(10., v * 0.05);
}

void limi_04_postamp_setter(number v) {
    this->limi_04_postamp = rnbo_pow(10., v * 0.05);
}

void limi_04_threshold_setter(number v) {
    this->limi_04_threshold = rnbo_pow(10., v * 0.05);
}

number limi_01_dc1_next(number x, number gain) {
    number y = x - this->limi_01_dc1_xm1 + this->limi_01_dc1_ym1 * gain;
    this->limi_01_dc1_xm1 = x;
    this->limi_01_dc1_ym1 = y;
    return y;
}

void limi_01_dc1_reset() {
    this->limi_01_dc1_xm1 = 0;
    this->limi_01_dc1_ym1 = 0;
}

void limi_01_dc1_dspsetup() {
    this->limi_01_dc1_reset();
}

number limi_01_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_01_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_reset();
        }
    }
}

void limi_01_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_dspsetup();
        }
    }
}

void limi_01_reset() {
    this->limi_01_recover = (number)1000 / (this->limi_01_release * this->samplerate());

    {
        this->limi_01_recover *= 0.707;
    }
}

void limi_01_dspsetup(bool force) {
    if ((bool)(this->limi_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_01_reset();
    this->limi_01_setupDone = true;
    this->limi_01_dc1_dspsetup();
}

number limi_02_dc1_next(number x, number gain) {
    number y = x - this->limi_02_dc1_xm1 + this->limi_02_dc1_ym1 * gain;
    this->limi_02_dc1_xm1 = x;
    this->limi_02_dc1_ym1 = y;
    return y;
}

void limi_02_dc1_reset() {
    this->limi_02_dc1_xm1 = 0;
    this->limi_02_dc1_ym1 = 0;
}

void limi_02_dc1_dspsetup() {
    this->limi_02_dc1_reset();
}

number limi_02_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_02_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_reset();
        }
    }
}

void limi_02_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_dspsetup();
        }
    }
}

void limi_02_reset() {
    this->limi_02_recover = (number)1000 / (this->limi_02_release * this->samplerate());

    {
        this->limi_02_recover *= 0.707;
    }
}

void limi_02_dspsetup(bool force) {
    if ((bool)(this->limi_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_02_reset();
    this->limi_02_setupDone = true;
    this->limi_02_dc1_dspsetup();
}

number limi_03_dc1_next(number x, number gain) {
    number y = x - this->limi_03_dc1_xm1 + this->limi_03_dc1_ym1 * gain;
    this->limi_03_dc1_xm1 = x;
    this->limi_03_dc1_ym1 = y;
    return y;
}

void limi_03_dc1_reset() {
    this->limi_03_dc1_xm1 = 0;
    this->limi_03_dc1_ym1 = 0;
}

void limi_03_dc1_dspsetup() {
    this->limi_03_dc1_reset();
}

number limi_03_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_03_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_03_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_03_dc1_reset();
        }
    }
}

void limi_03_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_03_dc1_dspsetup();
        }
    }
}

void limi_03_reset() {
    this->limi_03_recover = (number)1000 / (this->limi_03_release * this->samplerate());

    {
        this->limi_03_recover *= 0.707;
    }
}

void limi_03_dspsetup(bool force) {
    if ((bool)(this->limi_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_03_reset();
    this->limi_03_setupDone = true;
    this->limi_03_dc1_dspsetup();
}

number limi_04_dc1_next(number x, number gain) {
    number y = x - this->limi_04_dc1_xm1 + this->limi_04_dc1_ym1 * gain;
    this->limi_04_dc1_xm1 = x;
    this->limi_04_dc1_ym1 = y;
    return y;
}

void limi_04_dc1_reset() {
    this->limi_04_dc1_xm1 = 0;
    this->limi_04_dc1_ym1 = 0;
}

void limi_04_dc1_dspsetup() {
    this->limi_04_dc1_reset();
}

number limi_04_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_04_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_04_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_04_dc1_reset();
        }
    }
}

void limi_04_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_04_dc1_dspsetup();
        }
    }
}

void limi_04_reset() {
    this->limi_04_recover = (number)1000 / (this->limi_04_release * this->samplerate());

    {
        this->limi_04_recover *= 0.707;
    }
}

void limi_04_dspsetup(bool force) {
    if ((bool)(this->limi_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_04_reset();
    this->limi_04_setupDone = true;
    this->limi_04_dc1_dspsetup();
}

number svf_tilde_01_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dFreq_prev);
    this->svf_tilde_01_dFreq_prev = x;
    return temp;
}

void svf_tilde_01_dFreq_dspsetup() {
    this->svf_tilde_01_dFreq_reset();
}

void svf_tilde_01_dFreq_reset() {
    this->svf_tilde_01_dFreq_prev = 0;
}

number svf_tilde_01_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dQ_prev);
    this->svf_tilde_01_dQ_prev = x;
    return temp;
}

void svf_tilde_01_dQ_dspsetup() {
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dQ_reset() {
    this->svf_tilde_01_dQ_prev = 0;
}

void svf_tilde_01_reset() {
    this->svf_tilde_01_low = 0;
    this->svf_tilde_01_high = 0;
    this->svf_tilde_01_band = 0;
    this->svf_tilde_01_notch = 0;
    this->svf_tilde_01_dFreq_reset();
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_01_reset();
    this->svf_tilde_01_setupDone = true;
    this->svf_tilde_01_dFreq_dspsetup();
    this->svf_tilde_01_dQ_dspsetup();
}

number mtof_01_innerMtoF_next(number midivalue, number tuning) {
    if (midivalue == this->mtof_01_innerMtoF_lastInValue && tuning == this->mtof_01_innerMtoF_lastTuning)
        return this->mtof_01_innerMtoF_lastOutValue;

    this->mtof_01_innerMtoF_lastInValue = midivalue;
    this->mtof_01_innerMtoF_lastTuning = tuning;
    number result = 0;

    {
        result = rnbo_exp(.057762265 * (midivalue - 69.0));
    }

    this->mtof_01_innerMtoF_lastOutValue = tuning * result;
    return this->mtof_01_innerMtoF_lastOutValue;
}

void mtof_01_innerMtoF_reset() {
    this->mtof_01_innerMtoF_lastInValue = 0;
    this->mtof_01_innerMtoF_lastOutValue = 0;
    this->mtof_01_innerMtoF_lastTuning = 0;
}

void mtof_01_innerScala_mid(int v) {
    this->mtof_01_innerScala_kbmMid = v;
    this->mtof_01_innerScala_updateRefFreq();
}

void mtof_01_innerScala_ref(int v) {
    this->mtof_01_innerScala_kbmRefNum = v;
    this->mtof_01_innerScala_updateRefFreq();
}

void mtof_01_innerScala_base(number v) {
    this->mtof_01_innerScala_kbmRefFreq = v;
    this->mtof_01_innerScala_updateRefFreq();
}

void mtof_01_innerScala_init() {
    list sclValid = {
        12,
        100,
        0,
        200,
        0,
        300,
        0,
        400,
        0,
        500,
        0,
        600,
        0,
        700,
        0,
        800,
        0,
        900,
        0,
        1000,
        0,
        1100,
        0,
        2,
        1
    };

    this->mtof_01_innerScala_updateScale(sclValid);
}

void mtof_01_innerScala_update(list scale, list map) {
    if (scale->length > 0) {
        this->mtof_01_innerScala_updateScale(scale);
    }

    if (map->length > 0) {
        this->mtof_01_innerScala_updateMap(map);
    }
}

number mtof_01_innerScala_mtof(number note) {
    if ((bool)(this->mtof_01_innerScala_lastValid) && this->mtof_01_innerScala_lastNote == note) {
        return this->mtof_01_innerScala_lastFreq;
    }

    array<int, 2> degoct = this->mtof_01_innerScala_applyKBM(note);
    number out = 0;

    if (degoct[1] > 0) {
        out = this->mtof_01_innerScala_applySCL(degoct[0], fract(note), this->mtof_01_innerScala_refFreq);
    }

    this->mtof_01_innerScala_updateLast(note, out);
    return out;
}

number mtof_01_innerScala_ftom(number hz) {
    if (hz <= 0.0) {
        return 0.0;
    }

    if ((bool)(this->mtof_01_innerScala_lastValid) && this->mtof_01_innerScala_lastFreq == hz) {
        return this->mtof_01_innerScala_lastNote;
    }

    array<number, 2> df = this->mtof_01_innerScala_hztodeg(hz);
    int degree = (int)(df[0]);
    number frac = df[1];
    number out = 0;

    if (this->mtof_01_innerScala_kbmSize == 0) {
        out = this->mtof_01_innerScala_kbmMid + degree;
    } else {
        array<int, 2> octdeg = this->mtof_01_innerScala_octdegree(degree, this->mtof_01_innerScala_kbmOctaveDegree);
        number oct = (number)(octdeg[0]);
        int index = (int)(octdeg[1]);
        Index entry = 0;

        for (Index i = 0; i < this->mtof_01_innerScala_kbmMapSize; i++) {
            if (index == this->mtof_01_innerScala_kbmValid[(Index)(i + this->mtof_01_innerScala_KBM_MAP_OFFSET)]) {
                entry = i;
                break;
            }
        }

        out = oct * this->mtof_01_innerScala_kbmSize + entry + this->mtof_01_innerScala_kbmMid;
    }

    out = out + frac;
    this->mtof_01_innerScala_updateLast(out, hz);
    return this->mtof_01_innerScala_lastNote;
}

int mtof_01_innerScala_updateScale(list scl) {
    if (scl->length < 1) {
        return 0;
    }

    number sclDataEntries = scl[0] * 2 + 1;

    if (sclDataEntries <= scl->length) {
        this->mtof_01_innerScala_lastValid = false;
        this->mtof_01_innerScala_sclExpMul = {};
        number last = 1;

        for (Index i = 1; i < sclDataEntries; i += 2) {
            const number c = (const number)(scl[(Index)(i + 0)]);
            const number d = (const number)(scl[(Index)(i + 1)]);

            if (d <= 0) {
                last = c / (number)1200;
            } else {
                last = rnbo_log2(c / d);
            }

            this->mtof_01_innerScala_sclExpMul->push(last);
        }

        this->mtof_01_innerScala_sclOctaveMul = last;
        this->mtof_01_innerScala_sclEntryCount = (int)(this->mtof_01_innerScala_sclExpMul->length);

        if (scl->length >= sclDataEntries + 3) {
            this->mtof_01_innerScala_kbmMid = (int)(scl[(Index)(sclDataEntries + 2)]);
            this->mtof_01_innerScala_kbmRefNum = (int)(scl[(Index)(sclDataEntries + 1)]);
            this->mtof_01_innerScala_kbmRefFreq = scl[(Index)(sclDataEntries + 0)];
            this->mtof_01_innerScala_kbmSize = (int)(0);
        }

        this->mtof_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

int mtof_01_innerScala_updateMap(list kbm) {
    if (kbm->length == 1 && kbm[0] == 0.0) {
        kbm = {0.0, 0.0, 0.0, 60.0, 69.0, 440.0};
    }

    if (kbm->length >= 6 && kbm[0] >= 0.0) {
        this->mtof_01_innerScala_lastValid = false;
        Index size = (Index)(kbm[0]);
        int octave = 12;

        if (kbm->length > 6) {
            octave = (int)(kbm[6]);
        }

        if (size > 0 && kbm->length < this->mtof_01_innerScala_KBM_MAP_OFFSET) {
            return 0;
        }

        this->mtof_01_innerScala_kbmSize = (int)(size);
        this->mtof_01_innerScala_kbmMin = (int)(kbm[1]);
        this->mtof_01_innerScala_kbmMax = (int)(kbm[2]);
        this->mtof_01_innerScala_kbmMid = (int)(kbm[3]);
        this->mtof_01_innerScala_kbmRefNum = (int)(kbm[4]);
        this->mtof_01_innerScala_kbmRefFreq = kbm[5];
        this->mtof_01_innerScala_kbmOctaveDegree = octave;
        this->mtof_01_innerScala_kbmValid = kbm;
        this->mtof_01_innerScala_kbmMapSize = (kbm->length - this->mtof_01_innerScala_KBM_MAP_OFFSET > kbm->length ? kbm->length : (kbm->length - this->mtof_01_innerScala_KBM_MAP_OFFSET < 0 ? 0 : kbm->length - this->mtof_01_innerScala_KBM_MAP_OFFSET));
        this->mtof_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

void mtof_01_innerScala_updateLast(number note, number freq) {
    this->mtof_01_innerScala_lastValid = true;
    this->mtof_01_innerScala_lastNote = note;
    this->mtof_01_innerScala_lastFreq = freq;
}

array<number, 2> mtof_01_innerScala_hztodeg(number hz) {
    number hza = rnbo_abs(hz);

    number octave = rnbo_floor(
        rnbo_log2(hza / this->mtof_01_innerScala_refFreq) / this->mtof_01_innerScala_sclOctaveMul
    );

    int i = 0;
    number frac = 0;
    number n = 0;

    for (; i < this->mtof_01_innerScala_sclEntryCount; i++) {
        number c = this->mtof_01_innerScala_applySCLOctIndex(octave, i + 0, 0.0, this->mtof_01_innerScala_refFreq);
        n = this->mtof_01_innerScala_applySCLOctIndex(octave, i + 1, 0.0, this->mtof_01_innerScala_refFreq);

        if (c <= hza && hza < n) {
            if (c != hza) {
                frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
            }

            break;
        }
    }

    if (i == this->mtof_01_innerScala_sclEntryCount && n != hza) {
        number c = n;
        n = this->mtof_01_innerScala_applySCLOctIndex(octave + 1, 0, 0.0, this->mtof_01_innerScala_refFreq);
        frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
    }

    number deg = i + octave * this->mtof_01_innerScala_sclEntryCount;

    {
        deg = rnbo_fround((deg + frac) * 1 / (number)1) * 1;
        frac = 0.0;
    }

    return {deg, frac};
}

array<int, 2> mtof_01_innerScala_octdegree(int degree, int count) {
    int octave = 0;
    int index = 0;

    if (degree < 0) {
        octave = -(1 + (-1 - degree) / count);
        index = -degree % count;

        if (index > 0) {
            index = count - index;
        }
    } else {
        octave = degree / count;
        index = degree % count;
    }

    return {octave, index};
}

array<int, 2> mtof_01_innerScala_applyKBM(number note) {
    if ((this->mtof_01_innerScala_kbmMin == this->mtof_01_innerScala_kbmMax && this->mtof_01_innerScala_kbmMax == 0) || (note >= this->mtof_01_innerScala_kbmMin && note <= this->mtof_01_innerScala_kbmMax)) {
        int degree = (int)(rnbo_floor(note - this->mtof_01_innerScala_kbmMid));

        if (this->mtof_01_innerScala_kbmSize == 0) {
            return {degree, 1};
        }

        array<int, 2> octdeg = this->mtof_01_innerScala_octdegree(degree, this->mtof_01_innerScala_kbmSize);
        int octave = (int)(octdeg[0]);
        Index index = (Index)(octdeg[1]);

        if (this->mtof_01_innerScala_kbmMapSize > index) {
            degree = (int)(this->mtof_01_innerScala_kbmValid[(Index)(this->mtof_01_innerScala_KBM_MAP_OFFSET + index)]);

            if (degree >= 0) {
                return {degree + octave * this->mtof_01_innerScala_kbmOctaveDegree, 1};
            }
        }
    }

    return {-1, 0};
}

number mtof_01_innerScala_applySCL(int degree, number frac, number refFreq) {
    array<int, 2> octdeg = this->mtof_01_innerScala_octdegree(degree, this->mtof_01_innerScala_sclEntryCount);
    return this->mtof_01_innerScala_applySCLOctIndex(octdeg[0], octdeg[1], frac, refFreq);
}

number mtof_01_innerScala_applySCLOctIndex(number octave, int index, number frac, number refFreq) {
    number p = 0;

    if (index > 0) {
        p = this->mtof_01_innerScala_sclExpMul[(Index)(index - 1)];
    }

    if (frac > 0) {
        p = this->linearinterp(frac, p, this->mtof_01_innerScala_sclExpMul[(Index)index]);
    } else if (frac < 0) {
        p = this->linearinterp(-frac, this->mtof_01_innerScala_sclExpMul[(Index)index], p);
    }

    return refFreq * rnbo_pow(2, p + octave * this->mtof_01_innerScala_sclOctaveMul);
}

void mtof_01_innerScala_updateRefFreq() {
    this->mtof_01_innerScala_lastValid = false;
    int refOffset = (int)(this->mtof_01_innerScala_kbmRefNum - this->mtof_01_innerScala_kbmMid);

    if (refOffset == 0) {
        this->mtof_01_innerScala_refFreq = this->mtof_01_innerScala_kbmRefFreq;
    } else {
        int base = (int)(this->mtof_01_innerScala_kbmSize);

        if (base < 1) {
            base = this->mtof_01_innerScala_sclEntryCount;
        }

        array<int, 2> octdeg = this->mtof_01_innerScala_octdegree(refOffset, base);
        number oct = (number)(octdeg[0]);
        int index = (int)(octdeg[1]);

        if (base > 0) {
            oct = oct + rnbo_floor(index / base);
            index = index % base;
        }

        if (index >= 0 && index < this->mtof_01_innerScala_kbmSize) {
            if (index < this->mtof_01_innerScala_kbmMapSize) {
                index = (int)(this->mtof_01_innerScala_kbmValid[(Index)((Index)(index) + this->mtof_01_innerScala_KBM_MAP_OFFSET)]);
            } else {
                index = -1;
            }
        }

        if (index < 0 || index > this->mtof_01_innerScala_sclExpMul->length)
            {} else {
            number p = 0;

            if (index > 0) {
                p = this->mtof_01_innerScala_sclExpMul[(Index)(index - 1)];
            }

            this->mtof_01_innerScala_refFreq = this->mtof_01_innerScala_kbmRefFreq / rnbo_pow(2, p + oct * this->mtof_01_innerScala_sclOctaveMul);
        }
    }
}

void mtof_01_innerScala_reset() {
    this->mtof_01_innerScala_internal = true;
    this->mtof_01_innerScala_lastValid = false;
    this->mtof_01_innerScala_lastNote = 0;
    this->mtof_01_innerScala_lastFreq = 0;
    this->mtof_01_innerScala_sclEntryCount = 0;
    this->mtof_01_innerScala_sclOctaveMul = 1;
    this->mtof_01_innerScala_sclExpMul = {};
    this->mtof_01_innerScala_kbmValid = {0, 0, 0, 60, 69, 440};
    this->mtof_01_innerScala_kbmMid = 60;
    this->mtof_01_innerScala_kbmRefNum = 69;
    this->mtof_01_innerScala_kbmRefFreq = 440;
    this->mtof_01_innerScala_kbmSize = 0;
    this->mtof_01_innerScala_kbmMin = 0;
    this->mtof_01_innerScala_kbmMax = 0;
    this->mtof_01_innerScala_kbmOctaveDegree = 12;
    this->mtof_01_innerScala_kbmMapSize = 0;
    this->mtof_01_innerScala_refFreq = 261.63;
}

void mtof_01_init() {
    this->mtof_01_innerScala_update(this->mtof_01_scale, this->mtof_01_map);
}

list midiparse_01_parser_next(number midiin) {
    int midivalue = (int)(trunc(midiin));
    list result = list(-1);
    number resetByte1 = false;

    if (midivalue > 127) {
        this->midiparse_01_parser_status = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0xFF);
        this->midiparse_01_parser_byte1 = -1;
    } else {
        switch ((int)((BinOpInt)this->midiparse_01_parser_status & (BinOpInt)0xF0)) {
        case 0xB0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    2,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xA0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    1,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xE0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                number pitchbend;

                {
                    number val = this->midiparse_01_parser_byte1 + ((BinOpInt)(((BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F)) << imod_nocast((UBinOpInt)7, 32)));

                    {
                        val -= 0x2000;
                        pitchbend = val / (number)8192;
                    }
                }

                result = {
                    5,
                    pitchbend,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xD0:
            {
            result = {
                4,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x90:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xC0:
            {
            result = {
                3,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x80:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    0,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        default:
            {
            result = {-1};
            }
        }
    }

    if (result->length > 1) {
        result->push(this->midiparse_01_parser_status);
        result->push(this->midiparse_01_parser_byte1);
        result->push(midivalue);
    }

    if ((bool)(resetByte1)) {
        this->midiparse_01_parser_byte1 = -1;
    }

    return result;
}

void midiparse_01_parser_reset() {
    this->midiparse_01_parser_status = -1;
    this->midiparse_01_parser_byte1 = -1;
}

number gen_01_phasor_0_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_01_phasor_0_conv;

    if (this->gen_01_phasor_0_currentPhase < 0.)
        this->gen_01_phasor_0_currentPhase = 1. + this->gen_01_phasor_0_currentPhase;

    if (this->gen_01_phasor_0_currentPhase > 1.)
        this->gen_01_phasor_0_currentPhase = this->gen_01_phasor_0_currentPhase - 1.;

    number tmp = this->gen_01_phasor_0_currentPhase;
    this->gen_01_phasor_0_currentPhase += pincr;
    return tmp;
}

void gen_01_phasor_0_reset() {
    this->gen_01_phasor_0_currentPhase = 0;
}

void gen_01_phasor_0_dspsetup() {
    this->gen_01_phasor_0_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_phasor_0_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void adsr_01_dspsetup(bool force) {
    if ((bool)(this->adsr_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->adsr_01_mspersamp = (number)1000 / this->sr;
    this->adsr_01_setupDone = true;
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    limi_01_bypass = 0;
    limi_01_dcblock = 0;
    limi_01_lookahead = 100;
    limi_01_lookahead_setter(limi_01_lookahead);
    limi_01_preamp = 0;
    limi_01_preamp_setter(limi_01_preamp);
    limi_01_postamp = 0;
    limi_01_postamp_setter(limi_01_postamp);
    limi_01_threshold = 0;
    limi_01_threshold_setter(limi_01_threshold);
    limi_01_release = 1000;
    limi_02_bypass = 0;
    limi_02_dcblock = 0;
    limi_02_lookahead = 100;
    limi_02_lookahead_setter(limi_02_lookahead);
    limi_02_preamp = 0;
    limi_02_preamp_setter(limi_02_preamp);
    limi_02_postamp = 0;
    limi_02_postamp_setter(limi_02_postamp);
    limi_02_threshold = 0;
    limi_02_threshold_setter(limi_02_threshold);
    limi_02_release = 1000;
    limi_03_bypass = 0;
    limi_03_dcblock = 0;
    limi_03_lookahead = 100;
    limi_03_lookahead_setter(limi_03_lookahead);
    limi_03_preamp = 0;
    limi_03_preamp_setter(limi_03_preamp);
    limi_03_postamp = 0;
    limi_03_postamp_setter(limi_03_postamp);
    limi_03_threshold = 0;
    limi_03_threshold_setter(limi_03_threshold);
    limi_03_release = 1000;
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0;
    downsamp_tilde_01_x = 0;
    downsamp_tilde_01_downSample = 0;
    limi_04_bypass = 0;
    limi_04_dcblock = 0;
    limi_04_lookahead = 100;
    limi_04_lookahead_setter(limi_04_lookahead);
    limi_04_preamp = 0;
    limi_04_preamp_setter(limi_04_preamp);
    limi_04_postamp = 0;
    limi_04_postamp_setter(limi_04_postamp);
    limi_04_threshold = 0;
    limi_04_threshold_setter(limi_04_threshold);
    limi_04_release = 1000;
    svf_tilde_01_x = 0;
    svf_tilde_01_freq = 0;
    svf_tilde_01_q = 0;
    mtof_01_base = 440;
    unpack_01_out1 = 0;
    unpack_01_out2 = 0;
    midiin_01_port = 0;
    gen_01_in1 = 0;
    param_01_value = 17;
    param_02_value = 500;
    param_03_value = 0.5;
    adsr_01_trigger_number = 0;
    adsr_01_attack = 0;
    adsr_01_decay = 0;
    adsr_01_sustain = 0;
    adsr_01_release = 0;
    adsr_01_legato = 0;
    expr_01_in1 = 0;
    expr_01_in2 = 127;
    expr_01_out1 = 0;
    param_04_value = 0;
    param_05_value = 500;
    param_06_value = 0.5;
    param_07_value = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    limi_01_last = 0;
    limi_01_lookaheadIndex = 0;
    limi_01_recover = 0;
    limi_01_lookaheadInv = 0;
    limi_01_dc1_xm1 = 0;
    limi_01_dc1_ym1 = 0;
    limi_01_setupDone = false;
    limi_02_last = 0;
    limi_02_lookaheadIndex = 0;
    limi_02_recover = 0;
    limi_02_lookaheadInv = 0;
    limi_02_dc1_xm1 = 0;
    limi_02_dc1_ym1 = 0;
    limi_02_setupDone = false;
    limi_03_last = 0;
    limi_03_lookaheadIndex = 0;
    limi_03_recover = 0;
    limi_03_lookaheadInv = 0;
    limi_03_dc1_xm1 = 0;
    limi_03_dc1_ym1 = 0;
    limi_03_setupDone = false;
    downsamp_tilde_01_lastDownSample = 0;
    downsamp_tilde_01_current = 0;
    downsamp_tilde_01_hold = 0;
    limi_04_last = 0;
    limi_04_lookaheadIndex = 0;
    limi_04_recover = 0;
    limi_04_lookaheadInv = 0;
    limi_04_dc1_xm1 = 0;
    limi_04_dc1_ym1 = 0;
    limi_04_setupDone = false;
    svf_tilde_01_low = 0;
    svf_tilde_01_high = 0;
    svf_tilde_01_band = 0;
    svf_tilde_01_notch = 0;
    svf_tilde_01__freq = 0.001;
    svf_tilde_01__q = 0.001;
    svf_tilde_01_dFreq_prev = 0;
    svf_tilde_01_dQ_prev = 0;
    svf_tilde_01_setupDone = false;
    mtof_01_innerMtoF_lastInValue = 0;
    mtof_01_innerMtoF_lastOutValue = 0;
    mtof_01_innerMtoF_lastTuning = 0;
    mtof_01_innerScala_internal = true;
    mtof_01_innerScala_lastValid = false;
    mtof_01_innerScala_lastNote = 0;
    mtof_01_innerScala_lastFreq = 0;
    mtof_01_innerScala_sclEntryCount = 0;
    mtof_01_innerScala_sclOctaveMul = 1;
    mtof_01_innerScala_kbmValid = { 0, 0, 0, 60, 69, 440 };
    mtof_01_innerScala_kbmMid = 60;
    mtof_01_innerScala_kbmRefNum = 69;
    mtof_01_innerScala_kbmRefFreq = 440;
    mtof_01_innerScala_kbmSize = 0;
    mtof_01_innerScala_kbmMin = 0;
    mtof_01_innerScala_kbmMax = 0;
    mtof_01_innerScala_kbmOctaveDegree = 12;
    mtof_01_innerScala_kbmMapSize = 0;
    mtof_01_innerScala_refFreq = 261.63;
    midiparse_01_parser_status = -1;
    midiparse_01_parser_byte1 = -1;
    gen_01_phasor_0_currentPhase = 0;
    gen_01_phasor_0_conv = 0;
    gen_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    adsr_01_phase = 3;
    adsr_01_mspersamp = 0;
    adsr_01_time = 0;
    adsr_01_lastTriggerVal = 0;
    adsr_01_amplitude = 0;
    adsr_01_outval = 0;
    adsr_01_startingpoint = 0;
    adsr_01_triggerBuf = nullptr;
    adsr_01_triggerValueBuf = nullptr;
    adsr_01_setupDone = false;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    param_07_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number limi_01_bypass;
    number limi_01_dcblock;
    number limi_01_lookahead;
    number limi_01_preamp;
    number limi_01_postamp;
    number limi_01_threshold;
    number limi_01_release;
    number limi_02_bypass;
    number limi_02_dcblock;
    number limi_02_lookahead;
    number limi_02_preamp;
    number limi_02_postamp;
    number limi_02_threshold;
    number limi_02_release;
    number limi_03_bypass;
    number limi_03_dcblock;
    number limi_03_lookahead;
    number limi_03_preamp;
    number limi_03_postamp;
    number limi_03_threshold;
    number limi_03_release;
    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number downsamp_tilde_01_x;
    number downsamp_tilde_01_downSample;
    number limi_04_bypass;
    number limi_04_dcblock;
    number limi_04_lookahead;
    number limi_04_preamp;
    number limi_04_postamp;
    number limi_04_threshold;
    number limi_04_release;
    number svf_tilde_01_x;
    number svf_tilde_01_freq;
    number svf_tilde_01_q;
    list mtof_01_midivalue;
    list mtof_01_scale;
    list mtof_01_map;
    number mtof_01_base;
    number unpack_01_out1;
    number unpack_01_out2;
    number midiin_01_port;
    number gen_01_in1;
    number param_01_value;
    number param_02_value;
    number param_03_value;
    number adsr_01_trigger_number;
    number adsr_01_attack;
    number adsr_01_decay;
    number adsr_01_sustain;
    number adsr_01_release;
    number adsr_01_legato;
    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    number param_07_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[3];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    SampleValue limi_01_lookaheadBuffers[1][512] = { };
    SampleValue limi_01_gainBuffer[512] = { };
    number limi_01_last;
    int limi_01_lookaheadIndex;
    number limi_01_recover;
    number limi_01_lookaheadInv;
    number limi_01_dc1_xm1;
    number limi_01_dc1_ym1;
    bool limi_01_setupDone;
    SampleValue limi_02_lookaheadBuffers[1][512] = { };
    SampleValue limi_02_gainBuffer[512] = { };
    number limi_02_last;
    int limi_02_lookaheadIndex;
    number limi_02_recover;
    number limi_02_lookaheadInv;
    number limi_02_dc1_xm1;
    number limi_02_dc1_ym1;
    bool limi_02_setupDone;
    SampleValue limi_03_lookaheadBuffers[1][512] = { };
    SampleValue limi_03_gainBuffer[512] = { };
    number limi_03_last;
    int limi_03_lookaheadIndex;
    number limi_03_recover;
    number limi_03_lookaheadInv;
    number limi_03_dc1_xm1;
    number limi_03_dc1_ym1;
    bool limi_03_setupDone;
    number downsamp_tilde_01_lastDownSample;
    int downsamp_tilde_01_current;
    number downsamp_tilde_01_hold;
    SampleValue limi_04_lookaheadBuffers[1][512] = { };
    SampleValue limi_04_gainBuffer[512] = { };
    number limi_04_last;
    int limi_04_lookaheadIndex;
    number limi_04_recover;
    number limi_04_lookaheadInv;
    number limi_04_dc1_xm1;
    number limi_04_dc1_ym1;
    bool limi_04_setupDone;
    number svf_tilde_01_low;
    number svf_tilde_01_high;
    number svf_tilde_01_band;
    number svf_tilde_01_notch;
    number svf_tilde_01__freq;
    number svf_tilde_01__q;
    number svf_tilde_01_dFreq_prev;
    number svf_tilde_01_dQ_prev;
    bool svf_tilde_01_setupDone;
    number mtof_01_innerMtoF_lastInValue;
    number mtof_01_innerMtoF_lastOutValue;
    number mtof_01_innerMtoF_lastTuning;
    Float64BufferRef mtof_01_innerMtoF_buffer;
    bool mtof_01_innerScala_internal;
    const Index mtof_01_innerScala_KBM_MAP_OFFSET = 7;
    bool mtof_01_innerScala_lastValid;
    number mtof_01_innerScala_lastNote;
    number mtof_01_innerScala_lastFreq;
    int mtof_01_innerScala_sclEntryCount;
    number mtof_01_innerScala_sclOctaveMul;
    list mtof_01_innerScala_sclExpMul;
    list mtof_01_innerScala_kbmValid;
    int mtof_01_innerScala_kbmMid;
    int mtof_01_innerScala_kbmRefNum;
    number mtof_01_innerScala_kbmRefFreq;
    int mtof_01_innerScala_kbmSize;
    int mtof_01_innerScala_kbmMin;
    int mtof_01_innerScala_kbmMax;
    int mtof_01_innerScala_kbmOctaveDegree;
    Index mtof_01_innerScala_kbmMapSize;
    number mtof_01_innerScala_refFreq;
    int midiparse_01_parser_status;
    int midiparse_01_parser_byte1;
    number gen_01_phasor_0_currentPhase;
    number gen_01_phasor_0_conv;
    bool gen_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    Int adsr_01_phase;
    number adsr_01_mspersamp;
    number adsr_01_time;
    number adsr_01_lastTriggerVal;
    number adsr_01_amplitude;
    number adsr_01_outval;
    number adsr_01_startingpoint;
    signal adsr_01_triggerBuf;
    signal adsr_01_triggerValueBuf;
    bool adsr_01_setupDone;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    number param_07_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef RNBODefaultMtofLookupTable256;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* createRnboYoi()
{
    return new RnboYoi();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr RnboYoiFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return createRnboYoi;
}

} // end RNBO namespace

