/*
  ==============================================================================

    FeatureAnalysis.cpp
    Created: 4 Aug 2017 9:53:11am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "FeatureAnalysis.h"


FeatureAnalysis::FeatureAnalysis(const DBConnector& db) : db_(db)
{
    sampleRate_ = 44100;
    frameSize_ = 2048;
    hopSize_ = 256;
    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();
    
    audio_ = factory.create("MonoLoader",
                            "sampleRate", sampleRate_);
    
    audioEq_ = factory.create("EqloudLoader",
                              "sampleRate", sampleRate_);
    
    spectral_ = factory.create("LowLevelSpectralExtractor",
                               "frameSize", frameSize_,
                               "hopSize", hopSize_,
                               "sampleRate", sampleRate_);
    
    spectralEq_ = factory.create("LowLevelSpectralEqloudExtractor",
                                 "frameSize", frameSize_,
                                 "hopSize", hopSize_,
                                 "sampleRate", sampleRate_);
    
    frameCutter_ = factory.create("FrameCutter",
                                  "frameSize", 64,
                                  "hopSize", 32);
    
    startStopSilence_ = factory.create("StartStopSilence");
    
    std::vector<std::string> stats = {"mean", "stdev"};
    aggr_ = factory.create("PoolAggregator",
                           "defaultStats", stats);
    
    envelope_ = factory.create("Envelope");
    lat_ = factory.create("LogAttackTime");
    centroid_ = factory.create("Centroid");
    trimmer_ = factory.create("Trimmer");
    rms_ = factory.create("RMS");
}


FeatureAnalysis::~FeatureAnalysis()
{
    delete audio_;
    delete audioEq_;
    delete spectral_;
    delete spectralEq_;
    delete aggr_;
    delete frameCutter_;
    delete startStopSilence_;
    delete envelope_;
    delete lat_;
    delete centroid_;
    delete trimmer_;
    delete rms_;
    
    essentia::shutdown();
}


void FeatureAnalysis::run(Sample::Ptr sample, OwnedArray<TimeSegmentation> &segmentations)
{
    resetFactoryAlgorithms();
    
    // Load audio for sample
    std::vector<Real> audioBuffer;
    std::vector<Real> audioEqBuffer;
    loadAudio(sample, audioBuffer, audio_);
    loadAudio(sample, audioEqBuffer, audioEq_);
    
    // Remove silence at the beginning and end of sample
    computeSampleStartAndStopTime(sample, audioBuffer);
    trimAudioBuffer(audioBuffer, sample->getStartTime(), sample->getStopTime());
    trimAudioBuffer(audioEqBuffer, sample->getStartTime(), sample->getStopTime());
    
    // Calculate audio features on entire sample length
    Pool pool;
    computeTemporalFeatures(audioEqBuffer, pool);
    computeSpectralEqLoudFeatures(audioEqBuffer, pool);
    computeSpectralFeatures(audioBuffer, pool);
    
    // Iterate through the required segmentations, creating and saving the segmented feature pool
    for (auto segmentation = segmentations.begin(); segmentation != segmentations.end(); ++segmentation)
    {
        Pool segmentedPool;
        computeSegmentPool(audioEqBuffer, **segmentation, pool, segmentedPool);
        savePool(sample, segmentedPool, **segmentation);
    }
}


void FeatureAnalysis::resetFactoryAlgorithms()
{
    audio_->reset();
    audioEq_->reset();
    spectral_->reset();
    spectralEq_->reset();
    aggr_->reset();
    frameCutter_->reset();
    startStopSilence_->reset();
    envelope_->reset();
    centroid_->reset();
    trimmer_->reset();
    rms_->reset();
}


void FeatureAnalysis::loadAudio(Sample::Ptr sample, std::vector<Real> &buffer, Algorithm* loader)
{
    try
    {
        loader->configure("filename", sample->getFile().getFullPathName().toStdString());
        loader->output("audio").set(buffer);
        loader->compute();
    }
    catch (std::exception& e)
    {
        // Something is wrong with the sample if we can't load it -- mark it and exclude from future analysis
        sample->setExclude(true);
        sample->updateSave(db_);
        throw std::exception(e);
    }
}


void FeatureAnalysis::computeSampleStartAndStopTime(Sample::Ptr sample, std::vector<Real>& buffer)
{
    std::vector<Real> frame;
    int startFrame;
    int stopFrame;
    int hopSize = 32;
    
    frameCutter_->input("signal").set(buffer);
    frameCutter_->configure("hopSize", hopSize);
    frameCutter_->output("frame").set(frame);
    startStopSilence_->input("frame").set(frame);
    startStopSilence_->output("startFrame").set(startFrame);
    startStopSilence_->output("stopFrame").set(stopFrame);
    
    while(true)
    {
        frameCutter_->compute();
        if (!frame.size())
        {
            break;
        }
        startStopSilence_->compute();
    }
    
    // Save the detected onset and end time for the sample
    sample->setStartTime(double(startFrame * hopSize) / sampleRate_);
    sample->setStopTime(double(stopFrame * hopSize) / sampleRate_);
}


void FeatureAnalysis::trimAudioBuffer(std::vector<Real> &buffer, Real startTrim, Real endTrim)
{
    std::vector<Real> trimBuffer;
    
    trimmer_->configure("startTime", startTrim);
    trimmer_->configure("endTime", endTrim);
    
    trimmer_->input("signal").set(buffer);
    trimmer_->output("signal").set(trimBuffer);
    trimmer_->compute();
    
    buffer = trimBuffer;
}


void FeatureAnalysis::computeTemporalFeatures(std::vector<Real>& buffer, Pool &pool)
{
    std::vector<Real> envelopeBuffer;
    Real lat;
    Real attackStart;
    Real attackStop;
    Real tc;
    
    envelope_->input("signal").set(buffer);
    envelope_->output("signal").set(envelopeBuffer);
    lat_->input("signal").set(envelopeBuffer);
    lat_->output("logAttackTime").set(lat);
    lat_->output("attackStart").set(attackStart);
    lat_->output("attackStop").set(attackStop);
    
    centroid_->configure("range", Real((buffer.size() - 1)) / sampleRate_);
    centroid_->input("array").set(envelopeBuffer);
    centroid_->output("centroid").set(tc);
    
    envelope_->compute();
    lat_->compute();
    centroid_->compute();
    
    pool.add("lat", lat);
    pool.add("temporal_centroid", tc);
}


void FeatureAnalysis::computeSpectralEqLoudFeatures(std::vector<Real> &buffer, Pool &pool)
{
    // Collection variables for output of spectral extractor
    std::vector<Real> dissonance;
    std::vector<std::vector<Real>> sccoeffs;
    std::vector<std::vector<Real>> scvalleys;
    std::vector<Real> spectral_centroid;
    std::vector<Real> spectral_kurtosis;
    std::vector<Real> spectral_skewness;
    std::vector<Real> spectral_spread;
    
    // Connections for spectral extractor
    spectralEq_->input("signal").set(buffer);
    spectralEq_->output("dissonance").set(dissonance);
    spectralEq_->output("sccoeffs").set(sccoeffs);
    spectralEq_->output("scvalleys").set(scvalleys);
    spectralEq_->output("spectral_centroid").set(spectral_centroid);
    spectralEq_->output("spectral_kurtosis").set(spectral_kurtosis);
    spectralEq_->output("spectral_skewness").set(spectral_skewness);
    spectralEq_->output("spectral_spread").set(spectral_spread);
    
    // Compute low level spectral indicators with equal loudness filter
    spectralEq_->compute();
    
    // Save features into pool
    pool.append("dissonance", dissonance);
    pool.merge("sccoeffs", sccoeffs, "replace");
    pool.merge("scvalleys", scvalleys, "replace");
    pool.append("spectral_centroid", spectral_centroid);
    pool.append("spectral_kurtosis", spectral_kurtosis);
    pool.append("spectral_skewness", spectral_skewness);
    pool.append("spectral_spread", spectral_spread);
}


void FeatureAnalysis::computeSpectralFeatures(std::vector<Real> &buffer, Pool &pool)
{
    // Collecting variables for spectral extractor output
    std::vector<std::vector<Real>> barkbands;
    std::vector<Real> barkbands_kurtosis;
    std::vector<Real> barkbands_skewness;
    std::vector<Real> barkbands_spread;
    std::vector<Real> hfc;
    std::vector<std::vector<Real>> mfcc;
    std::vector<Real> pitch;
    std::vector<Real> pitch_instantaneous_confidence;
    std::vector<Real> pitch_salience;
    std::vector<Real> silence_rate_20dB;
    std::vector<Real> silence_rate_30dB;
    std::vector<Real> silence_rate_60dB;
    std::vector<Real> spectral_complexity;
    std::vector<Real> spectral_crest;
    std::vector<Real> spectral_decrease;
    std::vector<Real> spectral_energy;
    std::vector<Real> spectral_energyband_low;
    std::vector<Real> spectral_energyband_middle_low;
    std::vector<Real> spectral_energyband_middle_high;
    std::vector<Real> spectral_energyband_high;
    std::vector<Real> spectral_flatness_db;
    std::vector<Real> spectral_flux;
    std::vector<Real> spectral_rms;
    std::vector<Real> spectral_rolloff;
    std::vector<Real> spectral_strongpeak;
    std::vector<Real> zerocrossingrate;
    std::vector<Real> inharmonicity;
    std::vector<std::vector<Real>> tristimulus;
    std::vector<Real> oddtoevenharmonicenergyratio;
    
    // --- Connections for spectral extractor
    spectral_->input("signal").set(buffer);
    spectral_->output("barkbands").set(barkbands);
    spectral_->output("barkbands_skewness").set(barkbands_skewness);
    spectral_->output("barkbands_kurtosis").set(barkbands_kurtosis);
    spectral_->output("barkbands_spread").set(barkbands_spread);
    spectral_->output("hfc").set(hfc);
    spectral_->output("mfcc").set(mfcc);
    spectral_->output("pitch").set(pitch);
    spectral_->output("pitch_instantaneous_confidence").set(pitch_instantaneous_confidence);
    spectral_->output("pitch_salience").set(pitch_salience);
    spectral_->output("silence_rate_20dB").set(silence_rate_20dB);
    spectral_->output("silence_rate_30dB").set(silence_rate_30dB);
    spectral_->output("silence_rate_60dB").set(silence_rate_60dB);
    spectral_->output("spectral_complexity").set(spectral_complexity);
    spectral_->output("spectral_crest").set(spectral_crest);
    spectral_->output("spectral_decrease").set(spectral_decrease);
    spectral_->output("spectral_energy").set(spectral_energy);
    spectral_->output("spectral_energyband_low").set(spectral_energyband_low);
    spectral_->output("spectral_energyband_middle_low").set(spectral_energyband_middle_low);
    spectral_->output("spectral_energyband_middle_high").set(spectral_energyband_middle_high);
    spectral_->output("spectral_energyband_high").set(spectral_energyband_high);
    spectral_->output("spectral_flatness_db").set(spectral_flatness_db);
    spectral_->output("spectral_flux").set(spectral_flux);
    spectral_->output("spectral_rms").set(spectral_rms);
    spectral_->output("spectral_rolloff").set(spectral_rolloff);
    spectral_->output("spectral_strongpeak").set(spectral_strongpeak);
    spectral_->output("zerocrossingrate").set(zerocrossingrate);
    spectral_->output("inharmonicity").set(inharmonicity);
    spectral_->output("tristimulus").set(tristimulus);
    spectral_->output("oddtoevenharmonicenergyratio").set(oddtoevenharmonicenergyratio);
    
    // Compute low level spectral indicators
    spectral_->compute();
    
    // Add features into pool
    pool.merge("barkbands", barkbands, "replace");
    pool.append("barkbands_kurtosis", barkbands_kurtosis);
    pool.append("barkbands_skewness", barkbands_skewness);
    pool.append("barkbands_spread", barkbands_spread);
    pool.append("hfc", hfc);
    pool.merge("mfcc", mfcc, "replace");
    pool.append("pitch", pitch);
    pool.append("pitch_instantaneous_confidence", pitch_instantaneous_confidence);
    pool.append("pitch_salience", pitch_salience);
    pool.append("silence_rate_20dB", silence_rate_20dB);
    pool.append("silence_rate_30dB", silence_rate_30dB);
    pool.append("silence_rate_60dB", silence_rate_60dB);
    pool.append("spectral_complexity", spectral_complexity);
    pool.append("spectral_crest", spectral_crest);
    pool.append("spectral_decrease", spectral_decrease);
    pool.append("spectral_energy", spectral_energy);
    pool.append("spectral_energyband_low", spectral_energyband_low);
    pool.append("spectral_energyband_middle_low", spectral_energyband_middle_low);
    pool.append("spectral_energyband_middle_high", spectral_energyband_middle_high);
    pool.append("spectral_energyband_high", spectral_energyband_high);
    pool.append("spectral_flatness_db", spectral_flatness_db);
    pool.append("spectral_flux", spectral_flux);
    pool.append("spectral_rms", spectral_rms);
    pool.append("spectral_rolloff", spectral_rolloff);
    pool.append("spectral_strongpeak", spectral_strongpeak);
    pool.append("inharmonicity", inharmonicity);
    pool.merge("tristimulus", tristimulus, "replace");
    pool.append("oddtoevenharmonicenergyratio", oddtoevenharmonicenergyratio);
}


void FeatureAnalysis::computeSegmentPool(std::vector<Real>& buffer, TimeSegmentation& segmentation, Pool& inputPool, Pool& outputPool)
{
    envelope_->reset();
    lat_->reset();
    trimmer_->reset();
    
    // Apply time segmentation
    std::vector<Real> envelopeBuffer;
    std::vector<Real> trimBuffer;
    Real windowStart;
    Real windowEnd;
    Real latUnused;
    
    envelope_->input("signal").set(buffer);
    envelope_->output("signal").set(envelopeBuffer);
    
    lat_->configure("startAttackThreshold", segmentation.start > 0.9 ? 0.9 : segmentation.start);
    lat_->configure("stopAttackThreshold", 1.0);
    lat_->input("signal").set(envelopeBuffer);
    lat_->output("logAttackTime").set(latUnused);
    lat_->output("attackStart").set(windowStart);
    lat_->output("attackStop").set(windowEnd);
    
    envelope_->compute();
    lat_->compute();
    
    windowStart = segmentation.start >= 0.9 ? windowEnd : windowStart;
    
    if (segmentation.length > 0)
    {
        trimmer_->configure("startTime", windowStart);
        trimmer_->configure("endTime", windowStart + segmentation.length);
        trimmer_->input("signal").set(buffer);
        trimmer_->output("signal").set(trimBuffer);
        trimmer_->compute();
    } else
    {
        trimBuffer = buffer;
    }
    
    // Calculat RMS on trimmed sample
    Real rms;
    rms_->input("array").set(trimBuffer);
    rms_->output("rms").set(rms);
    rms_->compute();
    rms = 20*log10(rms);
    
    outputPool.add("rms", rms);
    
    long frameStart = (windowStart * sampleRate_) / hopSize_;
    long frameEnd = ((windowStart + segmentation.length) * sampleRate_) / hopSize_;
    
    const std::map<std::string, std::vector<Real>>& inputVectors = inputPool.getRealPool();
    const std::map<std::string, std::vector<std::vector<Real>>>& input2DVectors = inputPool.getVectorRealPool();
    
    segmentPool(inputVectors, outputPool, frameStart, frameEnd);
    segmentPool(input2DVectors, outputPool, frameStart, frameEnd);
}


template<typename T>
void FeatureAnalysis::segmentPool(const std::map<std::string, std::vector<T>> &inputFeatures, Pool &outputPool, long startFrame, long endFrame)
{
    // Iterate over all values of the map, segments them, and add segments to the output pool
    for (auto const& feature : inputFeatures)
    {
        size_t numFrames = inputFeatures.at(feature.first).size();
        if (numFrames <= 1)
        {
            outputPool.merge(feature.first, feature.second, "replace");
            continue;
        }
        
        long startIndex = startFrame;
        long endIndex = endFrame >= numFrames ? numFrames - 1 : endFrame;
        
        auto start = feature.second.begin() + startIndex;
        auto end = feature.second.begin() + endIndex;
        
        outputPool.merge(feature.first, std::vector<T>(start, end), "replace");
    }
}


void FeatureAnalysis::savePool(Sample::Ptr sample, Pool &pool, TimeSegmentation &segmentation)
{
    DBG("Start: " << segmentation.start << " End:" << segmentation.length);
    
    // Stats aggregration
    Pool aggrPool;
    
    aggr_->input("input").set(pool);
    aggr_->output("output").set(aggrPool);
    aggr_->compute();
    
    const std::map<std::string, std::vector<Real>>& vectorResults = aggrPool.getRealPool();
    const std::map<std::string, Real>& realResults = aggrPool.getSingleRealPool();
    
    AnalysisObject::Ptr analysis = new AnalysisObject(0, sample->getId(), segmentation.start, segmentation.length);
    
    // Save extracted features into the analysis object for this sample
    analysis->bark_1_mean = vectorResults.at("barkbands.mean").at(0);
    analysis->bark_2_mean = vectorResults.at("barkbands.mean").at(1);
    analysis->bark_3_mean = vectorResults.at("barkbands.mean").at(2);
    analysis->bark_4_mean = vectorResults.at("barkbands.mean").at(3);
    analysis->bark_5_mean = vectorResults.at("barkbands.mean").at(4);
    analysis->bark_6_mean = vectorResults.at("barkbands.mean").at(5);
    analysis->bark_7_mean = vectorResults.at("barkbands.mean").at(6);
    analysis->bark_8_mean = vectorResults.at("barkbands.mean").at(7);
    analysis->bark_9_mean = vectorResults.at("barkbands.mean").at(8);
    analysis->bark_10_mean = vectorResults.at("barkbands.mean").at(9);
    analysis->bark_11_mean = vectorResults.at("barkbands.mean").at(10);
    analysis->bark_12_mean = vectorResults.at("barkbands.mean").at(11);
    analysis->bark_13_mean = vectorResults.at("barkbands.mean").at(12);
    analysis->bark_14_mean = vectorResults.at("barkbands.mean").at(13);
    analysis->bark_15_mean = vectorResults.at("barkbands.mean").at(14);
    analysis->bark_16_mean = vectorResults.at("barkbands.mean").at(15);
    analysis->bark_17_mean = vectorResults.at("barkbands.mean").at(16);
    analysis->bark_18_mean = vectorResults.at("barkbands.mean").at(17);
    analysis->bark_19_mean = vectorResults.at("barkbands.mean").at(18);
    analysis->bark_20_mean = vectorResults.at("barkbands.mean").at(19);
    analysis->bark_21_mean = vectorResults.at("barkbands.mean").at(20);
    analysis->bark_22_mean = vectorResults.at("barkbands.mean").at(21);
    analysis->bark_23_mean = vectorResults.at("barkbands.mean").at(22);
    analysis->bark_24_mean = vectorResults.at("barkbands.mean").at(23);
    analysis->bark_25_mean = vectorResults.at("barkbands.mean").at(24);
    analysis->bark_26_mean = vectorResults.at("barkbands.mean").at(25);
    analysis->bark_27_mean = vectorResults.at("barkbands.mean").at(26);
    
    analysis->bark_1_dev = vectorResults.at("barkbands.stdev").at(0);
    analysis->bark_2_dev = vectorResults.at("barkbands.stdev").at(1);
    analysis->bark_3_dev = vectorResults.at("barkbands.stdev").at(2);
    analysis->bark_4_dev = vectorResults.at("barkbands.stdev").at(3);
    analysis->bark_5_dev = vectorResults.at("barkbands.stdev").at(4);
    analysis->bark_6_dev = vectorResults.at("barkbands.stdev").at(5);
    analysis->bark_7_dev = vectorResults.at("barkbands.stdev").at(6);
    analysis->bark_8_dev = vectorResults.at("barkbands.stdev").at(7);
    analysis->bark_9_dev = vectorResults.at("barkbands.stdev").at(8);
    analysis->bark_10_dev = vectorResults.at("barkbands.stdev").at(9);
    analysis->bark_11_dev = vectorResults.at("barkbands.stdev").at(10);
    analysis->bark_12_dev = vectorResults.at("barkbands.stdev").at(11);
    analysis->bark_13_dev = vectorResults.at("barkbands.stdev").at(12);
    analysis->bark_14_dev = vectorResults.at("barkbands.stdev").at(13);
    analysis->bark_15_dev = vectorResults.at("barkbands.stdev").at(14);
    analysis->bark_16_dev = vectorResults.at("barkbands.stdev").at(15);
    analysis->bark_17_dev = vectorResults.at("barkbands.stdev").at(16);
    analysis->bark_18_dev = vectorResults.at("barkbands.stdev").at(17);
    analysis->bark_19_dev = vectorResults.at("barkbands.stdev").at(18);
    analysis->bark_20_dev = vectorResults.at("barkbands.stdev").at(19);
    analysis->bark_21_dev = vectorResults.at("barkbands.stdev").at(20);
    analysis->bark_22_dev = vectorResults.at("barkbands.stdev").at(21);
    analysis->bark_23_dev = vectorResults.at("barkbands.stdev").at(22);
    analysis->bark_24_dev = vectorResults.at("barkbands.stdev").at(23);
    analysis->bark_25_dev = vectorResults.at("barkbands.stdev").at(24);
    analysis->bark_26_dev = vectorResults.at("barkbands.stdev").at(25);
    analysis->bark_27_dev = vectorResults.at("barkbands.stdev").at(26);
    
    analysis->bark_kurtosis = realResults.at("barkbands_kurtosis.mean");
    analysis->bark_skewness = realResults.at("barkbands_skewness.mean");
    analysis->bark_spread = realResults.at("barkbands_spread.mean");
    
    analysis->bark_kurtosis_dev = realResults.at("barkbands_kurtosis.stdev");
    analysis->bark_skewness_dev = realResults.at("barkbands_skewness.stdev");
    analysis->bark_spread_dev = realResults.at("barkbands_spread.stdev");
    
    analysis->mfcc_1_mean = vectorResults.at("mfcc.mean").at(0);
    analysis->mfcc_2_mean = vectorResults.at("mfcc.mean").at(1);
    analysis->mfcc_3_mean = vectorResults.at("mfcc.mean").at(2);
    analysis->mfcc_4_mean = vectorResults.at("mfcc.mean").at(3);
    analysis->mfcc_5_mean = vectorResults.at("mfcc.mean").at(4);
    analysis->mfcc_6_mean = vectorResults.at("mfcc.mean").at(5);
    analysis->mfcc_7_mean = vectorResults.at("mfcc.mean").at(6);
    analysis->mfcc_8_mean = vectorResults.at("mfcc.mean").at(7);
    analysis->mfcc_9_mean = vectorResults.at("mfcc.mean").at(8);
    analysis->mfcc_10_mean = vectorResults.at("mfcc.mean").at(9);
    analysis->mfcc_11_mean = vectorResults.at("mfcc.mean").at(10);
    analysis->mfcc_12_mean = vectorResults.at("mfcc.mean").at(11);
    analysis->mfcc_13_mean = vectorResults.at("mfcc.mean").at(12);
    
    analysis->mfcc_1_dev = vectorResults.at("mfcc.stdev").at(0);
    analysis->mfcc_2_dev = vectorResults.at("mfcc.stdev").at(1);
    analysis->mfcc_3_dev = vectorResults.at("mfcc.stdev").at(2);
    analysis->mfcc_4_dev = vectorResults.at("mfcc.stdev").at(3);
    analysis->mfcc_5_dev = vectorResults.at("mfcc.stdev").at(4);
    analysis->mfcc_6_dev = vectorResults.at("mfcc.stdev").at(5);
    analysis->mfcc_7_dev = vectorResults.at("mfcc.stdev").at(6);
    analysis->mfcc_8_dev = vectorResults.at("mfcc.stdev").at(7);
    analysis->mfcc_9_dev = vectorResults.at("mfcc.stdev").at(8);
    analysis->mfcc_10_dev = vectorResults.at("mfcc.stdev").at(9);
    analysis->mfcc_11_dev = vectorResults.at("mfcc.stdev").at(10);
    analysis->mfcc_12_dev = vectorResults.at("mfcc.stdev").at(11);
    analysis->mfcc_13_dev = vectorResults.at("mfcc.stdev").at(12);
    
    analysis->hfc = realResults.at("hfc.mean");
    analysis->pitch_salience = realResults.at("pitch_salience.mean");
    analysis->spectral_complexity = realResults.at("spectral_complexity.mean");
    analysis->spectral_crest = realResults.at("spectral_crest.mean");
    analysis->spectral_decrease = realResults.at("spectral_decrease.mean");
    analysis->spectral_energy = realResults.at("spectral_energy.mean");
    analysis->spectral_energyband_low = realResults.at("spectral_energyband_low.mean");
    analysis->spectral_energyband_middle_low = realResults.at("spectral_energyband_middle_low.mean");
    analysis->spectral_energyband_middle_high = realResults.at("spectral_energyband_middle_high.mean");
    analysis->spectral_energyband_high = realResults.at("spectral_energyband_high.mean");
    analysis->spectral_flatness_db = realResults.at("spectral_flatness_db.mean");
    analysis->spectral_flux = realResults.at("spectral_flux.mean");
    analysis->spectral_rms = realResults.at("spectral_rms.mean");
    analysis->spectral_rolloff = realResults.at("spectral_rolloff.mean");
    analysis->spectral_strongpeak = realResults.at("spectral_strongpeak.mean");
    analysis->inharmonicity = realResults.at("inharmonicity.mean");
    
    analysis->hfc_dev = realResults.at("hfc.stdev");
    analysis->pitch_salience_dev = realResults.at("pitch_salience.stdev");
    analysis->spectral_complexity_dev = realResults.at("spectral_complexity.stdev");
    analysis->spectral_crest_dev = realResults.at("spectral_crest.stdev");
    analysis->spectral_decrease_dev = realResults.at("spectral_decrease.stdev");
    analysis->spectral_energy_dev = realResults.at("spectral_energy.stdev");
    analysis->spectral_energyband_low_dev = realResults.at("spectral_energyband_low.stdev");
    analysis->spectral_energyband_middle_low_dev = realResults.at("spectral_energyband_middle_low.stdev");
    analysis->spectral_energyband_middle_high_dev = realResults.at("spectral_energyband_middle_high.stdev");
    analysis->spectral_energyband_high_dev = realResults.at("spectral_energyband_high.stdev");
    analysis->spectral_flatness_db_dev = realResults.at("spectral_flatness_db.stdev");
    analysis->spectral_flux_dev = realResults.at("spectral_flux.stdev");
    analysis->spectral_rms_dev = realResults.at("spectral_rms.stdev");
    analysis->spectral_rolloff_dev = realResults.at("spectral_rolloff.stdev");
    analysis->spectral_strongpeak_dev = realResults.at("spectral_strongpeak.stdev");
    analysis->inharmonicity_dev = realResults.at("inharmonicity.stdev");
    
    analysis->tristimulus_1 = vectorResults.at("tristimulus.mean").at(0);
    analysis->tristimulus_2 = vectorResults.at("tristimulus.mean").at(1);
    analysis->tristimulus_3 = vectorResults.at("tristimulus.mean").at(2);
    
    analysis->tristimulus_1_dev = vectorResults.at("tristimulus.stdev").at(0);
    analysis->tristimulus_2_dev = vectorResults.at("tristimulus.stdev").at(1);
    analysis->tristimulus_3_dev = vectorResults.at("tristimulus.stdev").at(2);
    
    analysis->spectral_centroid = realResults.at("spectral_centroid.mean");
    analysis->spectral_kurtosis = realResults.at("spectral_kurtosis.mean");
    analysis->spectral_skewness = realResults.at("spectral_skewness.mean");
    analysis->spectral_spread = realResults.at("spectral_spread.mean");
    
    analysis->spectral_centroid_dev = realResults.at("spectral_centroid.stdev");
    analysis->spectral_kurtosis_dev = realResults.at("spectral_kurtosis.stdev");
    analysis->spectral_skewness_dev = realResults.at("spectral_skewness.stdev");
    analysis->spectral_spread_dev = realResults.at("spectral_spread.stdev");
    
    analysis->lat = realResults.at("lat.mean");
    analysis->temporal_centroid = realResults.at("temporal_centroid.mean");
    analysis->rms = realResults.at("rms.mean");
    
    analysis->save(db_);
}
